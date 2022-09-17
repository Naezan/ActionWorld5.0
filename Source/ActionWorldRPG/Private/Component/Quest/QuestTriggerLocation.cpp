// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestTriggerLocation.h"
#include "Components/SphereComponent.h"
#include "NavAreas/NavArea_Obstacle.h"
#include "Character/Player/PlayerBase.h"

#include "Component/Quest/QuestSystemComponent.h"

#include "WorldSystem/MonsterSpawner.h"

AQuestTriggerLocation::AQuestTriggerLocation()
{
	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->InitSphereRadius(385.f);
	Sphere->AreaClass = UNavArea_Obstacle::StaticClass();
}

void AQuestTriggerLocation::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<APlayerBase>(OtherActor))
	{
		if (QuestComponent != nullptr)
		{
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
}

void AQuestTriggerLocation::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
}
