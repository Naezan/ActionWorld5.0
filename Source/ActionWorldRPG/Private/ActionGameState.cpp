// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/AssetManager.h"

//GameStateComponent
#include "Component/GameState/BOutExperienceManagerComponent.h"
#include "Components/GameStateComponent.h"

//GameFeature
#include "GameFeaturesSubsystem.h"

AActionGameState::AActionGameState()
{
	//로딩컴포넌트 추가
	ExperienceManagerComponent = 
	CreateDefaultSubobject<UBOutExperienceManagerComponent>(
	TEXT("ExperienceManagerComponent")
	);
}

void AActionGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//게임 피쳐에의해서 컴포넌트를 추가받을수 있도록 합니다.
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AActionGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//어빌리티 시스템 컴포넌트 초기화
}

void AActionGameState::BeginPlay()
{
	//클라이언트마다 엑터가 준비되었다는 이벤트를 보냅니다.
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);

	Super::BeginPlay();
}

void AActionGameState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);

	//메인 게임피쳐를 비활성화합니다.
	if (ExperienceManagerComponent != nullptr)
	{
		ExperienceManagerComponent->LoadAndActivateGameFeaturePlugin(TEXT("BlackOutCore"), false);

	}

	if (LoadState == EActionLoadState::Loaded)
	{
		LoadState = EActionLoadState::Deactivated;
		UE_LOG(LogTemp, Warning, TEXT("LoadState : Deactivated"));

		//Deactivate GameFeatureAction
		//

		OnAllActionsDeactivated();
	}
}

void AActionGameState::AddPlayerState(APlayerState* PlayerState)
{
	Super::AddPlayerState(PlayerState);
}

void AActionGameState::RemovePlayerState(APlayerState* PlayerState)
{
	Super::RemovePlayerState(PlayerState);
}

bool AActionGameState::ShouldShowLoadingScreen(FString& OutReason) const
{
	if (LoadState != EActionLoadState::Loaded)
	{
		OutReason = TEXT("ActionGameState still loading");
		return true;
	}
	else
	{
		return false;
	}
}

void AActionGameState::SetLoadState(EActionLoadState NewLoadState)
{
	LoadState = NewLoadState;
}

EActionLoadState AActionGameState::GetLoadState() const
{
	return LoadState;
}

void AActionGameState::StartLoadGame()
{
	//false일 경우 중지
	check(LoadState == EActionLoadState::Unloaded);

	LoadState = EActionLoadState::Loading;
	UE_LOG(LogTemp, Warning, TEXT("LoadState : Loading"));

	UAssetManager& AssetManager = UAssetManager::Get();

	TSet<FSoftObjectPath> RawAssetList;
	const TSharedPtr<FStreamableHandle> RawLoadHandle = AssetManager.LoadAssetList(RawAssetList.Array(), FStreamableDelegate());

	FStreamableDelegate OnAssetsLoadDelegate = FStreamableDelegate::CreateUObject(this, &ThisClass::LoadCompleteGame);

	if (!RawLoadHandle.IsValid() || RawLoadHandle->HasLoadCompleted())
	{
		FStreamableHandle::ExecuteDelegate(OnAssetsLoadDelegate);
	}
	else
	{
		//로드가 완료될때 호출되는 델리게이트를 새롭게 바인딩합니다. 이 함수는 로드중일때만 작동합니다.
		RawLoadHandle->BindCompleteDelegate(OnAssetsLoadDelegate);

		//핸들이 취소된경우에 호출되는 델리게이트를 새롭게 바인딩합니다. 이 함수는 로드중일때만 작동합니다.
		RawLoadHandle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadDelegate]()
			{
				//함수 포인터가 유효한경우에 실행합니다. 함수가 실행된 경우 true를 반환합니다.
				OnAssetsLoadDelegate.ExecuteIfBound();
			}));
	}
}

void AActionGameState::LoadCompleteGame()
{
	check(LoadState == EActionLoadState::Loading);

	//Exparience 게임 피쳐를 로드 및 활성화합니다.
	if (ExperienceManagerComponent != nullptr)
	{
		LoadState = EActionLoadState::LoadingGameFeatures;
		UE_LOG(LogTemp, Warning, TEXT("LoadState : LoadingGameFeatures"));
		ExperienceManagerComponent->LoadAndActivateGameFeaturePlugin(TEXT("BlackOutCore"), true);
		OnExperienceFullLoadCompleted();
	}
	else
	{
		OnExperienceFullLoadCompleted();
	}
}

void AActionGameState::OnExperienceFullLoadCompleted()
{
	//로드하기전 게임피쳐 작업마무리
	check(LoadState != EActionLoadState::Loaded);

	//Exparience가 가지고 있는 액션 게임피쳐를 등록, 로드 및 활성화합니다.
	LoadState = EActionLoadState::ExcutingGameFeatureActions;
	UE_LOG(LogTemp, Warning, TEXT("LoadState : ExcutingGameFeatureActions"));

	LoadState = EActionLoadState::Loaded;
	UE_LOG(LogTemp, Warning, TEXT("LoadState : Loaded"));
}

void AActionGameState::OnAllActionsDeactivated()
{
	LoadState = EActionLoadState::Unloaded;
	UE_LOG(LogTemp, Warning, TEXT("LoadState : Unloaded"));
	ExperienceManagerComponent = nullptr;
}