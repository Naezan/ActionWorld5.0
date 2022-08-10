// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionGameState.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/AssetManager.h"
//#include "Components/GameStateComponent.h"

AActionGameState::AActionGameState()
{
	//�ε�������Ʈ �߰�
}

void AActionGameState::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	//���� ���Ŀ����ؼ� ������Ʈ�� �߰������� �ֵ��� �մϴ�.
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AActionGameState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//�����Ƽ �ý��� ������Ʈ �ʱ�ȭ
}

void AActionGameState::BeginPlay()
{
	//Ŭ���̾�Ʈ���� ���Ͱ� �غ�Ǿ��ٴ� �̺�Ʈ�� �����ϴ�.
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
	//false�� ��� ����
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
		//�ε尡 �Ϸ�ɶ� ȣ��Ǵ� ��������Ʈ�� ���Ӱ� ���ε��մϴ�. �� �Լ��� �ε����϶��� �۵��մϴ�.
		RawLoadHandle->BindCompleteDelegate(OnAssetsLoadDelegate);

		//�ڵ��� ��ҵȰ�쿡 ȣ��Ǵ� ��������Ʈ�� ���Ӱ� ���ε��մϴ�. �� �Լ��� �ε����϶��� �۵��մϴ�.
		RawLoadHandle->BindCancelDelegate(FStreamableDelegate::CreateLambda([OnAssetsLoadDelegate]()
			{
				//�Լ� �����Ͱ� ��ȿ�Ѱ�쿡 �����մϴ�. �Լ��� ����� ��� true�� ��ȯ�մϴ�.
				OnAssetsLoadDelegate.ExecuteIfBound();
			}));
	}
}

void AActionGameState::LoadCompleteGame()
{
	check(LoadState != EActionLoadState::Loaded);

	LoadState = EActionLoadState::Loaded;
}
