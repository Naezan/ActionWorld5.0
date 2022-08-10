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
				//비어 있지 않는 상태가 실패하면 로그를 뱉음
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

//Slate의 모든 입력을 막는 역할을 하는 인풋 프로세스
class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputPreProcessor() {}
	virtual ~FLoadingScreenInputPreProcessor() {}

	//에디터에 있을경우 false를 반환
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
	//클라이언트만이 로딩스크린을 가질 수 있다.
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();

	//데디 서버가 아닌경우
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

		//지정시간내에 체크포인트에 도달하지 못하면 정지된 위치를 확인할 수 있다.
		FThreadHeartBeat::Get().MonitorCheckpointStart(GetFName(), Settings->LogLoadingScreenHeartbeatInterval);

		//한번만 호출됩니다.
		ShowLoadingScreen();

		//처음 로딩할 때 로딩 로그간의 간격 셋팅
		if (Settings->LogLoadingScreenHeartbeatInterval > 0.f && (NextLogHeartBeatTime <= 0.0))
		{
			NextLogHeartBeatTime = Settings->LogLoadingScreenHeartbeatInterval;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("HideLoadingScreen"));
		//한번만 호출됩니다.
		HideLoadingScreen();

		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}
}

bool ULoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	//환경변수에 셋팅이 되어 있다면 로딩 스크린을 띄우자
	if (Settings->ForceLoadingScreenVisible)
	{
		return true;
	}

	const UGameInstance* LocalGameInstance = GetGameInstance();

	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		//월드정보가 없으면 로딩 스크린을 띄우자
		return true;
	}

	UWorld* World = Context->World();
	if (World == nullptr)
	{
		//월드가 존재하지 않으면 로딩 스크린을 띄우자
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
	{
		//게임스테이트가 리플리케이트 되지 않았으면 로딩 스크린을 띄우자
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		//현재 맵을 로드중이면 로딩 스크린을 띄우자
		return true;
	}

	if (!Context->TravelURL.IsEmpty())
	{
		//월드 이동을 보류중이라면 로딩 스크린을 띄우자
		return true;
	}

	//if (Context->PendingNetGame != nullptr)
	//{
	//	//다른 서버에 연결중이라면 로딩 스크린을 띄우자
	//	return true;
	//}

	if (!World->HasBegunPlay())
	{
		//게임 플레이가 시작되지 않았다면 로딩 스크린을 띄우자
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		//원활하게 전환하고 있다면? 로딩 스크린을 띄우자
		return true;
	}

	//게임 스테이트가 로딩 스크린이 필요한지
	if (ILoadingScreenInterface::ShouldShowLoadingScreen(GameState, DebugLoadingScreenReason))
	{
		return true;
	}

	//게임 스테이트 컴포넌트들이 로딩 스크린이 필요한지
	for (UActorComponent* TestComponent : GameState->GetComponents())
	{
		if (ILoadingScreenInterface::ShouldShowLoadingScreen(TestComponent, /*out*/ DebugLoadingScreenReason))
		{
			return true;
		}
	}

	//로딩 스크린 인터페이스를 가지고 있는 오브젝트들이 로딩스크린이 필요한지
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

				//플레이어 컨트롤러가 로딩스크린이 필요하면 띄우자
				if (ILoadingScreenInterface::ShouldShowLoadingScreen(PC, DebugLoadingScreenReason))
				{
					return true;
				}

				//플레이어 컨트롤러의 컴포넌트들이 로딩 스크린이 필요하면 띄우자
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

	//화면이 분할되어있고 로컬플레이어를 하나 이상 찾을 수 없다면 로딩 스크린을 띄우자
	if (bIsInSplitscreen && bMissingAnyLocalPC)
	{
		return true;
	}

	//화면이 분할되어 있지 않고 로컬플레이어를 단 하나도 못찾았다면 로딩 스크린을 띄우자
	if (!bIsInSplitscreen && !bFoundAnyLocalPC)
	{
		return true;
	}

	//로딩 스크린이 필요없다
	return false;
}

bool ULoadingScreenManager::ShouldShowLoadingScreen()
{
	//CDO 반환. 여기서
	const ULoadingScreenSettings* Settings = GetDefault<ULoadingScreenSettings>();

	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();

	//로딩 스크린이 더 필요하다면 추가로 로딩
	bool bWantToForceShowLoadingScreen = false;

	if (bNeedToShowLoadingScreen)
	{

		LoadingScreenDismissedTime = -1.0;
	}
	//강제 로딩 조건
	else
	{
		const double CurrentTime = FPlatformTime::Seconds();

		//에디터에 있지 않거나 에디터에 추가 지연시간을 부여하는지
		const bool bCanHoldLoadingScreen = (!GIsEditor ||
			Settings->HoldLoadingScreenAdditionalSecsEvenInEditor);
		//추가지연셋팅이 되어있다면 추가지연시간 사용
		const double HoldLoadingScreenAdditionalSecs = bCanHoldLoadingScreen ?
			Settings->HoldLoadingScreenAdditionalSecs : 0.0;

		if (LoadingScreenDismissedTime < 0.0)
		{
			LoadingScreenDismissedTime = CurrentTime;
		}
		//누적 스크린 아웃시간
		const double TimeSinceScreenDismissed = CurrentTime - LoadingScreenDismissedTime;

		//강제 지연 조건(추가 지연시간이 존재하고 누적시간이 추가지연시간을 초과하지 않은상태)
		if ((HoldLoadingScreenAdditionalSecs > 0.0) && TimeSinceScreenDismissed < HoldLoadingScreenAdditionalSecs)
		{
			//로딩중에 렌더링을 합니다.
			UGameViewportClient* GameViewportClient = GetGameInstance()->GetGameViewportClient();
			GameViewportClient->bDisableWorldRendering = false;

			bWantToForceShowLoadingScreen = true;
		}
	}

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool ULoadingScreenManager::IsShowingInitialLoadingScreen() const
{
	//등록된 모든 사전 로드 화면 저장 및 재생 / 중지를 처리
	//스크린 매니저의 인스턴스를 가져오거나 필요한경우 생성하기도 합니다.
	//RegisterPreLoadScreen함수로 등록된 프리로드 스크린을 관리합니다.
	//등록하지 않으면 아무것도 하지 않습니다.
	//더 많은 기능을 사용하고 싶다면 PreLoadScreen 대신 FPreLoadScreenBase을 대신 사용하면됩니다.
	FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get();
	return (PreLoadScreenManager != nullptr) && PreLoadScreenManager->HasValidActivePreLoadScreen();
}

void ULoadingScreenManager::ShowLoadingScreen()
{
	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	//이미 로딩 스크린이 로딩중이거나 로딩할 수 없다면
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

		//현재 아무런 함수가 추가되어 있지 않아 아무역할도 하지 않는다
		LoadingScreenVisibilityChanged.Broadcast(true);

		//환경변수에 이는 위젯 가져오기
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

		//최상위 뷰포트에 추가하기
		UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->LoadingScreenZOrder);

		//PSO 캐시 로드
		ChangePerformanceSettings(true);

		if (!GIsEditor || Settings->ForceTickLoadingScreenEvenInEditor)
		{
			//로딩 스크린이 바로 뜨게 하기 위해 틱함수를 임의적으로 한번 호출
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
		//GameViewportClient는 인스턴스당 하나가 생성됩니다. 일반적으로 유저당1개가 생성된다고 봅니다.
		//GameViewport는 플랫폼(PC)의 렌더링, 오디오, 입력 서브시스템을 위해서 사용됩니다.
		if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
		{
			//슬레이트에 추가된 모든 위젯을 검사해서 일치하는 위젯을 제거합니다.
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}
		LoadingScreenWidget.Reset();
	}
}

void ULoadingScreenManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		//인풋 프로세스가 로딩 스크린이 뜨는 동안 모든 인풋을 다 감싸게됩니다.
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
	//각각의 로컬플레이어의 인스턴스를 가져옵니다.
	UGameInstance* LocalGameInstance = GetGameInstance();
	//로컬 플레이어의 뷰포트인스턴스를 가져옵니다. 게임뷰포트클라이언트란 모든 게임뷰포트에 대한 인터페이스 입니다.
	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();

	//PSO 캐시 로드 셋팅
	FShaderPipelineCache::SetBatchMode(bEnableLoadingScreen ?
		FShaderPipelineCache::BatchMode::Fast : FShaderPipelineCache::BatchMode::Background);

	//로딩스크린이 뜰때와 사라질때는 렌더링을 하지 않습니다. 추가로딩시간이 있는경우에만 렌더링을 합니다.
	GameViewportClient->bDisableWorldRendering = bEnableLoadingScreen;

	//로컬플레이어의 뷰포트가 있는 월드를 가져옵니다.
	if (UWorld* ViewportWorld = GameViewportClient->GetWorld())
	{
		//퍼시스턴트 레벨의 월드셋팅을 가져옵니다.
		//퍼시스턴트 레벨이란 마스터레벨로 일반적으로는 PIE에서 열려있는 레벨을 말합니다.
		if (AWorldSettings* WorldSettings = ViewportWorld->GetWorldSettings(false, false))
		{
			//리플리케이트 되지 않는 bHighPriorityLoading(리플리케이트됨)의 사본으로
			//이 옵션이 켜져 있으면 현재 레벨의 백그라운드 로드에 더 많은 시간을 할당합니다.
			WorldSettings->bHighPriorityLoadingLocal = bEnableLoadingScreen;
		}
	}

	if (bEnableLoadingScreen)
	{
		double HangDurationMultiplier;

		//만약 시스템 환경변수에 값이 셋팅되어있으면 셋팅값을 불러온다. 현재는 아무런 셋팅이 없기때문에 1이다.
		if (!GConfig || !GConfig->GetDouble(TEXT("Core.System"), TEXT("LoadingScreenHangDurationMultiplier"), HangDurationMultiplier, GEngineIni))
		{
			HangDurationMultiplier = 1.0;
		}

		FThreadHeartBeat::Get().SetDurationMultiplier(HangDurationMultiplier);

		//로딩 스크린이 뜨고있는 동안 하트비트의 버벅임감지를 일시중단합니다.
		//버벅임이란 끊기는 현상을 말합니다. 로딩 스크린을 키고있는동안에는 자연스럽게 버벅임이 발생하기때문에
		//의도치않게 중단될 수 있으므로 히치 탐지를 하지 않도록 합니다.
		FGameThreadHitchHeartBeat::Get().SuspendHeartBeat();
	}
	else
	{
		FThreadHeartBeat::Get().SetDurationMultiplier(1.0);

		//로딩 스크린이 꺼지면 하트비트의 히치탐지를 재개합니다.
		FGameThreadHitchHeartBeat::Get().ResumeHeartBeat();
	}
}
