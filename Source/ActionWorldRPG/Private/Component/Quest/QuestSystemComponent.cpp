// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestSystemComponent.h"
#include "Component/Quest/QuestData.h"
#include "Kismet/GameplayStatics.h"

UQuestSystemComponent::UQuestSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//AssetManager를 사용하는 PrimaryData를 사용하면 에셋매니저에서 더 쉽게 접근할 수 있다.
	static ConstructorHelpers::FObjectFinder<UDataAsset> DefaultDataAsset(TEXT("/Game/Quest/PlayerQuests.PlayerQuests"));
	if (DefaultDataAsset.Succeeded())
	{
		UDataAsset* DataAsset = DefaultDataAsset.Object;
		Quests = Cast<UQuestData>(DataAsset);
	}
}


// Called when the game starts
void UQuestSystemComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UQuestSystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool UQuestSystemComponent::AddQuest(FQuest Quest)
{
	bool bIsSuccess = false;

	if (Quests == nullptr || Quest.IsEmpty())
		return bIsSuccess;

	//새로운 퀘스트 추가
	bIsSuccess = true;
	Quest.bActive = true;
	Quests->QuestDatas.Add(Quest);

	//기존 퀘스트 비활성화
	if (CurrentQuestID != -1)
	{
		Quests->QuestDatas[CurrentQuestID].bActive = false;
	}

	//활성화퀘스트 설정
	CurrentQuestID = Quests->QuestDatas.Num() - 1;
	ActiveQuest = Quest;

	//OnQuestAdded broadcast UI용
	//퀘스트의 데이터를 UI에 업데이트합니다.
	OnAddedRemovedQuest.Broadcast();

	//플레이어에게 퀘스트를 받았음을 전달합니다
	OnQuestNotify.Broadcast(false, Quest.QuestName);

	//UI에게 TTS할 텍스트를 전달합니다.
	SetTTSOnAddedCompletedQuest.Broadcast(Quest.QuestAddedText);

	//현재 퀘스트 타입이 Kill이라면
	if (Quest.QuestType == EQuestType::QT_Kill)
	{
		OnQuestStepUpdate.Broadcast();
	}

	return bIsSuccess;
}

void UQuestSystemComponent::CompleteQuest(bool& bSuccess)
{
	if (Quests == nullptr || CurrentQuestID == -1 || ActiveQuest.IsEmpty())
	{
		bSuccess = false;
		return;
	}

	//현재퀘스트 상태변경
	Quests->QuestDatas[CurrentQuestID].bComplete = true;
	Quests->QuestDatas[CurrentQuestID].bActive = false;

	//OnQuestCompleted broadcast UI용
	OnCompleteQuest.Broadcast();

	OnQuestNotify.Broadcast(true, Quests->QuestDatas[CurrentQuestID].QuestName);

	//UI에게 TTS할 텍스트를 전달합니다.
	SetTTSOnAddedCompletedQuest.Broadcast(Quests->QuestDatas[CurrentQuestID].QuestCompletedText);

	//보상이 존재한다면
	FReward NewReward = Quests->QuestDatas[CurrentQuestID].Reward;
	if (!NewReward.IsEmpty())
	{
		//다음퀘스트가 있다면
		if (NewReward.NextQuest != nullptr)
		{
			FName FirstName = NewReward.NextQuest->GetRowNames()[0];
			FQuest NewQuest = *NewReward.NextQuest->FindRow<FQuest>(FirstName, FString());
			if (!NewQuest.IsEmpty())
			{
				AddQuest(NewQuest);
			}
		}
		else
		{
			//최상단퀘스트 선택
			bSuccess = SelectQuest(CurrentQuestID - 1);// or SelectQuest(0);
		}
		//보상획득
		//TO DO
	}
	//보상이 없다면 그냥 퀘스트 변경
	else
	{
		//최상단퀘스트 선택
		bSuccess = SelectQuest(CurrentQuestID - 1);// or SelectQuest(0);
	}
}

void UQuestSystemComponent::CompleteQuestStep(bool& bSuccess, TSubclassOf<AActor> TargetActorClass)
{
	if (Quests == nullptr || CurrentQuestID == -1 || ActiveQuest.IsEmpty())
	{
		bSuccess = false;
		return;
	}

	if (ActiveQuest.MaxTargetCount == 0 || ActiveQuest.TargetClass == nullptr || ActiveQuest.TargetClass != TargetActorClass)
	{
		bSuccess = false;
		return;
	}

	ActiveQuest.TargetCount = ++Quests->QuestDatas[CurrentQuestID].TargetCount;
	
	//OnQuestStepUpdate UI
	OnQuestStepUpdate.Broadcast();

	if (ActiveQuest.TargetCount >= ActiveQuest.MaxTargetCount)
	{
		CompleteQuest(bSuccess);
	}
}

bool UQuestSystemComponent::SelectQuest(const int32 QuestID)
{
	bool bIsSuccess = false;
	if (Quests == nullptr)
		return bIsSuccess;

	//활성화할퀘스트가 선택할퀘스트와 같다면 아무것도 선택하지 않습니다.
	if (CurrentQuestID == QuestID)
	{
		return true;
	}

	if (QuestID >= 0 && !Quests->QuestDatas.IsEmpty() && Quests->QuestDatas.Num() > QuestID)
	{
		//만약 선택할퀘스트가 이미 완료되어있다면 그이전퀘스트를 선택한다.
		if (Quests->QuestDatas[QuestID].bComplete)
		{
			bIsSuccess = SelectQuest(QuestID - 1);
		}
		//선택할 퀘스트가 완료된 퀘스트가 아니라면 그 퀘스트를 활성화한다.
		else
		{
			bIsSuccess = true;
			CurrentQuestID = QuestID;
			Quests->QuestDatas[QuestID].bActive = true;
			ActiveQuest = Quests->QuestDatas[QuestID];

			//활성화된 퀘스트가 Kill타입이라면
			if (ActiveQuest.QuestType == EQuestType::QT_Kill)
			{
				OnQuestStepUpdate.Broadcast();
			}
		}

		//OnQuestSelected broadcast UI용
		//선택한 퀘스트를 UI에서 바꿔?줍니다?
		OnSelectQuest.Broadcast(ActiveQuest.QuestName, ActiveQuest.QeustDescription);
	}
	//퀘스트에 퀘스트가 없거나 인덱스가 초과했다면 퀘스트를 선택하지 않습니다.
	else
	{
		ResetCurrentQuest();

		//OnQuestCleared broadcast UI용
		//모든 퀘스트 클리어?
		//퀘스트를 Remove할일이 없기 때문에 호출될 일이 없습니다.
	}

	return bIsSuccess;
}

bool UQuestSystemComponent::RemoveQuest(const int32 QuestID)
{
	bool bIsSuccess = false;
	if (Quests == nullptr)
		return bIsSuccess;

	if (QuestID >= 0 && !Quests->QuestDatas.IsEmpty())
	{
		bIsSuccess = true;
		Quests->QuestDatas.RemoveAt(QuestID);

		//퀘스트를성공적으로 삭제했다면 퀘스트를 활성화합니다.
		if (QuestID <= CurrentQuestID)
		{
			SelectQuest(CurrentQuestID - 1);
		}
		////활성화중인퀘스트를 삭제했다면 최상단의퀘스트를 선택합니다.
		//else if (QuestID == CurrentQuestID)
		//{
		//	SelectQuest(0);
		//}

		//OnQuestRemoved broadcast UI용
		//퀘스트의 데이터를 UI에 업데이트합니다.
		OnAddedRemovedQuest.Broadcast();

		//플레이어에게 퀘스트를 제거했음을 전달합니다.
		//OnQuestNotify.Broadcast(false, Quest.QuestName);
	}

	return bIsSuccess;
}

void UQuestSystemComponent::ResetCurrentQuest()
{
	ActiveQuest.Clear();
	CurrentQuestID = -1;
}

