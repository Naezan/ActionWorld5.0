// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenManager.h"

#include "Engine/World.h"
#include "GameFramework/GameStateBase.h"

#include "PreLoadScreenManager.h"
#include "LoadingScreenInterface.h"
#include "LoadingScreenSettings.h"

#include "Framework/Application/IInputProcessor.h"
#include "HAL/ThreadHeartBeat.h"

//PSO. requirement RenderCore Module
#include "ShaderPipelineCache.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/Images/SThrobber.h"

DECLARE_LOG_CATEGORY_EXTERN(LoadingScreenLog, Log, All);
DEFINE_LOG_CATEGORY(LoadingScreenLog);

//interface function start
bool ILoadingScreenInterface::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject != nullptr)
	{
		if (ILoadingScreenInterface* LoadObserver = Cast<ILoadingScreenInterface>(TestObject))
		{
			FString ObserverReason;
			if (LoadObserver->ShouldShowLoadingScreen(ObserverReason))
			{
				//��� ���� �ʴ� ���°� �����ϸ� �α׸� ����
				if (ensureMsgf(!ObserverReason.IsEmpty(), TEXT("ObserverReason is Not Valid")))
				{
					OutReason = ObserverReason;
					return true;
				}
			}
		}
	}

	return false;
}
//interface function end

//Slate�� ��� �Է��� ���� ������ �ϴ� ��ǲ ���μ���
class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputPreProcessor() {}
	virtual ~FLoadingScreenInputPreProcessor() {}

	//�����Ϳ� ������� false�� ��ȯ
	bool CanEatInput() const
	{
		return !GIsEditor;
	}

	//~IInputProcess interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanEatInput(); }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanEatInput(); }
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanEatInput(); }
	//~End of IInputProcess interface
};

void ULoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);

	const UGameInstance* LocalGameInstance = GetGameInstance();
	check(LocalGameInstance);
}

void ULoadingScreenManager::Deinitialize()
{
	StopBlockingInput();

	RemoveWidgetFromViewport();

	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
}

bool ULoadingScreenManager::ShouldCreateSubsystem(UObject* Outer) const
{
	//Ŭ���̾�Ʈ���� �ε���ũ���� ���� �� �ִ�.
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();

	//���� ������ �ƴѰ��
	return !bIsServerWorld;
}

void ULoadingScreenManager::Tick(float DeltaTime)
{
	UpdateLoadingScreen();

	NextLogHeartBeatTime = FMath::Max(NextLogHeartBeatTime - DeltaTime, 0);
}

ETickableTickType ULoadingScreenManager::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULoadingScreenManager::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

TStatId ULoadingScreenManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenManager, STATGROUP_Tickables);
}

UWorld* ULoadingScreenManager::GetTickableGameObjectWorld() const
{
	return GetGameInstance()->GetWorld();
}

void ULoadingScreenManager::RegisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface)
{
	ExternalLoadingProcessors.Add(Interface.GetObject());
}

void ULoadingScreenManager::UnregisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface)
{
	ExternalLoadingProcessors.Remove(Interface.GetObject());
}

void ULoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance == GetGameInstance())
	{
		bCurrentlyInLoadMap = true;

		if (GEngine->IsInitialized())
		{
			UE_LOG(LogTemp, Warning, TEXT("PreLoadMap"));
			UpdateLoadingScreen();
		}
	}
}

void ULoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if ((World != nullptr) && (World->GetGameInstance() == GetGameInstance()))
	{
		bCurrentlyInLoadMap = false;
	}
}

void ULoadingScreenManager::UpdateLoadingScreen()
{
	if (ShouldShowLoadingScreen())
	{
		UE_LOG(LogTemp, Warning, TEXT("ShowLoadingScreen"));

		const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

		//�����ð����� üũ����Ʈ�� �������� ���ϸ� ������ ��ġ�� Ȯ���� �� �ִ�.
		FThreadHeartBeat::Get().MonitorCheckpointStart(GetFName(), Settings->LogLoadingScreenHeartbeatInterval);

		//�ѹ��� ȣ��˴ϴ�.
		ShowLoadingScreen();

		//ó�� �ε��� �� �ε� �αװ��� ���� ����
		if (Settings->LogLoadingScreenHeartbeatInterval > 0.f && (NextLogHeartBeatTime <= 0.0))
		{
			NextLogHeartBeatTime = Settings->LogLoadingScreenHeartbeatInterval;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HideLoadingScreen"));
		//�ѹ��� ȣ��˴ϴ�.
		HideLoadingScreen();

		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}
}

bool ULoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	//ȯ�溯���� ������ �Ǿ� �ִٸ� �ε� ��ũ���� �����
	if (Settings->ForceLoadingScreenVisible)
	{
		return true;
	}

	const UGameInstance* LocalGameInstance = GetGameInstance();

	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		//���������� ������ �ε� ��ũ���� �����
		return true;
	}

	UWorld* World = Context->World();
	if (World == nullptr)
	{
		//���尡 �������� ������ �ε� ��ũ���� �����
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
	{
		//���ӽ�����Ʈ�� ���ø�����Ʈ ���� �ʾ����� �ε� ��ũ���� �����
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		//���� ���� �ε����̸� �ε� ��ũ���� �����
		return true;
	}

	if (!Context->TravelURL.IsEmpty())
	{
		//���� �̵��� �������̶�� �ε� ��ũ���� �����
		return true;
	}

	//if (Context->PendingNetGame != nullptr)
	//{
	//	//�ٸ� ������ �������̶�� �ε� ��ũ���� �����
	//	return true;
	//}

	if (!World->HasBegunPlay())
	{
		//���� �÷��̰� ���۵��� �ʾҴٸ� �ε� ��ũ���� �����
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		//��Ȱ�ϰ� ��ȯ�ϰ� �ִٸ�? �ε� ��ũ���� �����
		return true;
	}

	//���� ������Ʈ�� �ε� ��ũ���� �ʿ�����
	if (ILoadingScreenInterface::ShouldShowLoadingScreen(GameState, DebugLoadingScreenReason))
	{
		return true;
	}

	//���� ������Ʈ ������Ʈ���� �ε� ��ũ���� �ʿ�����
	for (UActorComponent* TestComponent : GameState->GetComponents())
	{
		if (ILoadingScreenInterface::ShouldShowLoadingScreen(TestComponent, /*out*/ DebugLoadingScreenReason))
		{
			return true;
		}
	}

	//�ε� ��ũ�� �������̽��� ������ �ִ� ������Ʈ���� �ε���ũ���� �ʿ�����
	for (const TWeakInterfacePtr<ILoadingScreenInterface>& Processor : ExternalLoadingProcessors)
	{
		if (ILoadingScreenInterface::ShouldShowLoadingScreen(Processor.GetObject(), DebugLoadingScreenReason))
		{
			return true;
		}
	}

	bool bFoundAnyLocalPC = false;
	bool bMissingAnyLocalPC = false;

	for (ULocalPlayer* LP : LocalGameInstance->GetLocalPlayers())
	{
		if (LP != nullptr)
		{
			if (APlayerController* PC = LP->PlayerController)
			{
				bFoundAnyLocalPC = true;

				//�÷��̾� ��Ʈ�ѷ��� �ε���ũ���� �ʿ��ϸ� �����
				if (ILoadingScreenInterface::ShouldShowLoadingScreen(PC, DebugLoadingScreenReason))
				{
					return true;
				}

				//�÷��̾� ��Ʈ�ѷ��� ������Ʈ���� �ε� ��ũ���� �ʿ��ϸ� �����
				for (UActorComponent* LocalComponent : PC->GetComponents())
				{
					if (ILoadingScreenInterface::ShouldShowLoadingScreen(LocalComponent, DebugLoadingScreenReason))
					{
						return true;
					}
				}
			}
			else
			{
				bMissingAnyLocalPC = true;
			}
		}
	}

	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
	const bool bIsInSplitscreen = GameViewportClient->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None;

	//ȭ���� ���ҵǾ��ְ� �����÷��̾ �ϳ� �̻� ã�� �� ���ٸ� �ε� ��ũ���� �����
	if (bIsInSplitscreen && bMissingAnyLocalPC)
	{
		return true;
	}

	//ȭ���� ���ҵǾ� ���� �ʰ� �����÷��̾ �� �ϳ��� ��ã�Ҵٸ� �ε� ��ũ���� �����
	if (!bIsInSplitscreen && !bFoundAnyLocalPC)
	{
		return true;
	}

	//�ε� ��ũ���� �ʿ����
	return false;
}

bool ULoadingScreenManager::ShouldShowLoadingScreen()
{
	//CDO ��ȯ. ���⼭
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();

	//�ε� ��ũ���� �� �ʿ��ϴٸ� �߰��� �ε�
	bool bWantToForceShowLoadingScreen = false;

	if (bNeedToShowLoadingScreen)
	{

		LoadingScreenDismissedTime = -1.0;
	}
	//���� �ε� ����
	else
	{
		const double CurrentTime = FPlatformTime::Seconds();

		//�����Ϳ� ���� �ʰų� �����Ϳ� �߰� �����ð��� �ο��ϴ���
		const bool bCanHoldLoadingScreen = (!GIsEditor ||
			Settings->HoldLoadingScreenAdditionalSecsEvenInEditor);
		//�߰����������� �Ǿ��ִٸ� �߰������ð� ���
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ?
			Settings->HoldLoadingScreenAdditionalSecs : 0.0;

		if (LoadingScreenDismissedTime < 0.0)
		{
			LoadingScreenDismissedTime = CurrentTime;
		}
		//���� ��ũ�� �ƿ��ð�
		const double TimeSinceScreenDismissed = CurrentTime - LoadingScreenDismissedTime;

		//���� ���� ����(�߰� �����ð��� �����ϰ� �����ð��� �߰������ð��� �ʰ����� ��������)
		if ((HoldLoadingScreenAdditionalSecs > 0.0) && TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs)
		{
			//�ε��߿� �������� �մϴ�.
			UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient();
			GameViewportClient->bDisableWorldRendering = false;

			bWantToForceShowLoadingScreen = true;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool ULoadingScreenManager::IsShowingInitialLoadingScreen() const
{
	//��ϵ� ��� ���� �ε� ȭ�� ���� �� ��� / ������ ó��
	//��ũ�� �Ŵ����� �ν��Ͻ��� �������ų� �ʿ��Ѱ�� �����ϱ⵵ �մϴ�.
	//RegisterPreLoadScreen�Լ��� ��ϵ� �����ε� ��ũ���� �����մϴ�.
	//������� ������ �ƹ��͵� ���� �ʽ��ϴ�.
	//�� ���� ����� ����ϰ� �ʹٸ� PreLoadScreen ��� FPreLoadScreenBase�� ��� ����ϸ�˴ϴ�.
	FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get();
	return (PreLoadScreenManager != nullptr) && PreLoadScreenManager->HasValidActivePreLoadScreen();
}

void ULoadingScreenManager::ShowLoadingScreen()
{
	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	//�̹� �ε� ��ũ���� �ε����̰ų� �ε��� �� ���ٸ�
	if (FPreLoadScreenManager::Get() && FPreLoadScreenManager::Get()->HasActivePreLoadScreenType(EPreLoadScreenTypes::EngineLoadingScreen))
	{
		return;
	}

	LoadingScreenShownTime = FPlatformTime::Seconds();

	bCurrentlyShowingLoadingScreen = true;

	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	if (IsShowingInitialLoadingScreen())
	{
		//Do Nothing
		UE_LOG(LogTemp, Warning, TEXT("InitLoadingScreen"));
	}
	else
	{
		UGameInstance* LocalGameInstance = GetGameInstance();

		StartBlockingInput();

		//���� �ƹ��� �Լ��� �߰��Ǿ� ���� �ʾ� �ƹ����ҵ� ���� �ʴ´�
		LoadingScreenVisibilityChanged.Broadcast(true);

		//ȯ�溯���� �̴� ���� ��������
		TSubclassOf<UUserWidget> LoadingScreenWidgetClass = Settings->LoadingScreenWidget.TryLoadClass<UUserWidget>();
		if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*LocalGameInstance, LoadingScreenWidgetClass, NAME_None))
		{
			LoadingScreenWidget = UserWidget->TakeWidget();
		}
		else
		{
			LoadingScreenWidget = SNew(SThrobber);
		}

		UE_LOG(LogTemp, Warning, TEXT("LoadingScreenShown"));

		//�ֻ��� ����Ʈ�� �߰��ϱ�
		UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->LoadingScreenZOrder);

		//PSO ĳ�� �ε�
		ChangePerformanceSettings(true);

		if (!GIsEditor || Settings->ForceTickLoadingScreenEvenInEditor)
		{
			//�ε� ��ũ���� �ٷ� �߰� �ϱ� ���� ƽ�Լ��� ���������� �ѹ� ȣ��
			FSlateApplication::Get().Tick();
		}
	}
}

void ULoadingScreenManager::HideLoadingScreen()
{
	if (!bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	StopBlockingInput();

	if (IsShowingInitialLoadingScreen())
	{
		//Do Nothing
		UE_LOG(LogTemp, Warning, TEXT("InitLoadingScreen"));
	}
	else
	{
		GEngine->ForceGarbageCollection(true);

		RemoveWidgetFromViewport();

		UE_LOG(LogTemp, Warning, TEXT("LoadingScreenHiden"));

		ChangePerformanceSettings(false);

		LoadingScreenVisibilityChanged.Broadcast(false);
	}

	bCurrentlyShowingLoadingScreen = false;
}

void ULoadingScreenManager::RemoveWidgetFromViewport()
{
	UGameInstance* LocalGameInstance = GetGameInstance();
	if (LoadingScreenWidget.IsValid())
	{
		//GameViewportClient�� �ν��Ͻ��� �ϳ��� �����˴ϴ�. �Ϲ������� ������1���� �����ȴٰ� ���ϴ�.
		//GameViewport�� �÷���(PC)�� ������, �����, �Է� ����ý����� ���ؼ� ���˴ϴ�.
		if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
		{
			//������Ʈ�� �߰��� ��� ������ �˻��ؼ� ��ġ�ϴ� ������ �����մϴ�.
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}
		LoadingScreenWidget.Reset();
	}
}

void ULoadingScreenManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		//��ǲ ���μ����� �ε� ��ũ���� �ߴ� ���� ��� ��ǲ�� �� ���ΰԵ˴ϴ�.
		InputPreProcessor = MakeShareable<FLoadingScreenInputPreProcessor>(new FLoadingScreenInputPreProcessor());
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
	}
}

void ULoadingScreenManager::StopBlockingInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}

void ULoadingScreenManager::ChangePerformanceSettings(bool bEnableLoadingScreen)
{
	//������ �����÷��̾��� �ν��Ͻ��� �����ɴϴ�.
	UGameInstance* LocalGameInstance = GetGameInstance();
	//���� �÷��̾��� ����Ʈ�ν��Ͻ��� �����ɴϴ�. ���Ӻ���ƮŬ���̾�Ʈ�� ��� ���Ӻ���Ʈ�� ���� �������̽� �Դϴ�.
	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();

	//PSO ĳ�� �ε� ����
	FShaderPipelineCache::SetBatchMode(bEnableLoadingScreen ?
		FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	//�ε���ũ���� �㶧�� ��������� �������� ���� �ʽ��ϴ�. �߰��ε��ð��� �ִ°�쿡�� �������� �մϴ�.
	GameViewportClient->bDisableWorldRendering = bEnableLoadingScreen;

	//�����÷��̾��� ����Ʈ�� �ִ� ���带 �����ɴϴ�.
	if (UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		//�۽ý���Ʈ ������ ��������� �����ɴϴ�.
		//�۽ý���Ʈ �����̶� �����ͷ����� �Ϲ������δ� PIE���� �����ִ� ������ ���մϴ�.
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			//���ø�����Ʈ ���� �ʴ� bHighPriorityLoading(���ø�����Ʈ��)�� �纻����
			//�� �ɼ��� ���� ������ ���� ������ ��׶��� �ε忡 �� ���� �ð��� �Ҵ��մϴ�.
			WorldSettings->bHighPriorityLoadingLocal = bEnableLoadingScreen;
		}
	}

	if (bEnableLoadingScreen)
	{
		double HangDurationMultiplier;

		//���� �ý��� ȯ�溯���� ���� ���õǾ������� ���ð��� �ҷ��´�. ����� �ƹ��� ������ ���⶧���� 1�̴�.
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}

		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		//�ε� ��ũ���� �߰��ִ� ���� ��Ʈ��Ʈ�� �����Ӱ����� �Ͻ��ߴ��մϴ�.
		//�������̶� ����� ������ ���մϴ�. �ε� ��ũ���� Ű���ִµ��ȿ��� �ڿ������� �������� �߻��ϱ⶧����
		//�ǵ�ġ�ʰ� �ߴܵ� �� �����Ƿ� ��ġ Ž���� ���� �ʵ��� �մϴ�.
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);

		//�ε� ��ũ���� ������ ��Ʈ��Ʈ�� ��ġŽ���� �簳�մϴ�.
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}
