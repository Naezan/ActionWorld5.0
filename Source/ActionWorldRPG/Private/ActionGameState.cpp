// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/AssetManager.h"
//#include "Components/GameStateComponent.h"

AActionGameState::AActionGameState()
{
	//로딩컴포넌트 추가
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

	if (LoadState == EActionLoadState::Loaded)
		LoadState = EActionLoadState::Deactivated;

	Super::EndPlay(EndPlayReason);
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
	check(LoadState != EActionLoadState::Loaded);

	LoadState = EActionLoadState::Loaded;
}
