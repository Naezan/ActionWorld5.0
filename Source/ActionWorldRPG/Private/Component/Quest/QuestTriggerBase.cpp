// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestTriggerBase.h"
#include "Kismet/GameplayStatics.h"
#include "Character/Player/PlayerBase.h"
#include "Component/Quest/QuestSystemComponent.h"

// Sets default values
AQuestTriggerBase::AQuestTriggerBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	QuestType = EQuestType::QT_None;
}

// Called when the game starts or when spawned
void AQuestTriggerBase::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle waitHandle;
	GetWorldTimerManager().SetTimer(waitHandle, this, &AQuestTriggerBase::SetQuestComponent, 0.2f, false);
}

void AQuestTriggerBase::FinishQuest()
{
	const FQuest ActiveQuest = QuestComponent->GetActiveQuest();

	if (!ActiveQuest.bActive || ActiveQuest.bComplete || ActiveQuest.QuestName != QuestName || ActiveQuest.QuestType != QuestType)
		return;

	bool bSuccess;
	//바로 완료가능한 퀘스트
	if (ActiveQuest.MaxTargetCount == 0 && ActiveQuest.QuestType != EQuestType::QT_Kill)
	{
		QuestComponent->CompleteQuest(bSuccess);
	}
}

void AQuestTriggerBase::SetQuestComponent()
{
	APlayerBase* Player = Cast<APlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
		return;

	QuestComponent = Player->FindComponentByClass<UQuestSystemComponent>();
}
