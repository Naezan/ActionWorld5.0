// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestTriggerItem.h"
#include "Component/Quest/QuestSystemComponent.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"

#include "WorldSystem/MonsterSpawner.h"

AQuestTriggerItem::AQuestTriggerItem()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UActionAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* AQuestTriggerItem::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AQuestTriggerItem::InteractQuestItem()
{
	if (QuestComponent != nullptr)
	{
		//아이템전달?
		FinishQuest();

		//다음퀘스트가 순차적으로 존재한다면 다음퀘스트를 추가해줍니다.
		if (NextQuest != nullptr)
		{
			FName FirstName = NextQuest->GetRowNames()[0];
			FQuest NewQuest = *NextQuest->FindRow<FQuest>(FirstName, FString());
			if (!NewQuest.IsEmpty())
			{
				QuestComponent->AddQuest(NewQuest);
			}

			//KillType이라면 월드에서 받아온 몬스터 스포너 작동
			if (NewQuest.QuestType == EQuestType::QT_Kill)
			{
				for (int i = 0; i < MonsterSpawners.Num(); ++i)
				{
					IWorldInteractionInterface* WorldInteraction = Cast<IWorldInteractionInterface>(MonsterSpawners[i]);
					if (WorldInteraction != nullptr)
					{
						WorldInteraction->WorldInteractionProcess_Implementation();
					}
				}
			}
		}

		//액터 삭제
		Destroy();
	}
}
