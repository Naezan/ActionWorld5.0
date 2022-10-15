// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestSystemComponent.h"
#include "Component/Quest/QuestData.h"
#include "Kismet/GameplayStatics.h"

UQuestSystemComponent::UQuestSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	//AssetManager�� ����ϴ� PrimaryData�� ����ϸ� ���¸Ŵ������� �� ���� ������ �� �ִ�.
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

	//���ο� ����Ʈ �߰�
	bIsSuccess = true;
	Quest.bActive = true;
	Quests->QuestDatas.Add(Quest);

	//���� ����Ʈ ��Ȱ��ȭ
	if (CurrentQuestID != -1)
	{
		Quests->QuestDatas[CurrentQuestID].bActive = false;
	}

	//Ȱ��ȭ����Ʈ ����
	CurrentQuestID = Quests->QuestDatas.Num() - 1;
	ActiveQuest = Quest;

	//OnQuestAdded broadcast UI��
	//����Ʈ�� �����͸� UI�� ������Ʈ�մϴ�.
	OnAddedRemovedQuest.Broadcast();

	//�÷��̾�� ����Ʈ�� �޾����� �����մϴ�
	OnQuestNotify.Broadcast(false, Quest.QuestName);

	//UI���� TTS�� �ؽ�Ʈ�� �����մϴ�.
	SetTTSOnAddedCompletedQuest.Broadcast(Quest.QuestAddedText);

	//���� ����Ʈ Ÿ���� Kill�̶��
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

	//��������Ʈ ���º���
	Quests->QuestDatas[CurrentQuestID].bComplete = true;
	Quests->QuestDatas[CurrentQuestID].bActive = false;

	//OnQuestCompleted broadcast UI��
	OnCompleteQuest.Broadcast();

	OnQuestNotify.Broadcast(true, Quests->QuestDatas[CurrentQuestID].QuestName);

	//UI���� TTS�� �ؽ�Ʈ�� �����մϴ�.
	SetTTSOnAddedCompletedQuest.Broadcast(Quests->QuestDatas[CurrentQuestID].QuestCompletedText);

	//������ �����Ѵٸ�
	FReward NewReward = Quests->QuestDatas[CurrentQuestID].Reward;
	if (!NewReward.IsEmpty())
	{
		//��������Ʈ�� �ִٸ�
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
			//�ֻ������Ʈ ����
			bSuccess = SelectQuest(CurrentQuestID - 1);// or SelectQuest(0);
		}
		//����ȹ��
		//TO DO
	}
	//������ ���ٸ� �׳� ����Ʈ ����
	else
	{
		//�ֻ������Ʈ ����
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

	//Ȱ��ȭ������Ʈ�� ����������Ʈ�� ���ٸ� �ƹ��͵� �������� �ʽ��ϴ�.
	if (CurrentQuestID == QuestID)
	{
		return true;
	}

	if (QuestID >= 0 && !Quests->QuestDatas.IsEmpty() && Quests->QuestDatas.Num() > QuestID)
	{
		//���� ����������Ʈ�� �̹� �Ϸ�Ǿ��ִٸ� ����������Ʈ�� �����Ѵ�.
		if (Quests->QuestDatas[QuestID].bComplete)
		{
			bIsSuccess = SelectQuest(QuestID - 1);
		}
		//������ ����Ʈ�� �Ϸ�� ����Ʈ�� �ƴ϶�� �� ����Ʈ�� Ȱ��ȭ�Ѵ�.
		else
		{
			bIsSuccess = true;
			CurrentQuestID = QuestID;
			Quests->QuestDatas[QuestID].bActive = true;
			ActiveQuest = Quests->QuestDatas[QuestID];

			//Ȱ��ȭ�� ����Ʈ�� KillŸ���̶��
			if (ActiveQuest.QuestType == EQuestType::QT_Kill)
			{
				OnQuestStepUpdate.Broadcast();
			}
		}

		//OnQuestSelected broadcast UI��
		//������ ����Ʈ�� UI���� �ٲ�?�ݴϴ�?
		OnSelectQuest.Broadcast(ActiveQuest.QuestName, ActiveQuest.QeustDescription);
	}
	//����Ʈ�� ����Ʈ�� ���ų� �ε����� �ʰ��ߴٸ� ����Ʈ�� �������� �ʽ��ϴ�.
	else
	{
		ResetCurrentQuest();

		//OnQuestCleared broadcast UI��
		//��� ����Ʈ Ŭ����?
		//����Ʈ�� Remove������ ���� ������ ȣ��� ���� �����ϴ�.
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

		//����Ʈ������������ �����ߴٸ� ����Ʈ�� Ȱ��ȭ�մϴ�.
		if (QuestID <= CurrentQuestID)
		{
			SelectQuest(CurrentQuestID - 1);
		}
		////Ȱ��ȭ��������Ʈ�� �����ߴٸ� �ֻ��������Ʈ�� �����մϴ�.
		//else if (QuestID == CurrentQuestID)
		//{
		//	SelectQuest(0);
		//}

		//OnQuestRemoved broadcast UI��
		//����Ʈ�� �����͸� UI�� ������Ʈ�մϴ�.
		OnAddedRemovedQuest.Broadcast();

		//�÷��̾�� ����Ʈ�� ���������� �����մϴ�.
		//OnQuestNotify.Broadcast(false, Quest.QuestName);
	}

	return bIsSuccess;
}

void UQuestSystemComponent::ResetCurrentQuest()
{
	ActiveQuest.Clear();
	CurrentQuestID = -1;
}

