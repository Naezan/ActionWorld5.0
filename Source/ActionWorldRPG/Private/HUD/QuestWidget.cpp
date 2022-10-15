// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/QuestWidget.h"
#include "Character/Player/PlayerBase.h"
#include "Component/Quest/QuestSystemComponent.h"

void UQuestWidget::BindQuestDelegateFunction_Implementation(APlayerBase* PlayerCharacter)
{
    
    Player = PlayerCharacter;
    Player->GetQuestComponent()->OnAddedRemovedQuest.AddDynamic(this, &ThisClass::ShowActiveQuest);
    Player->GetQuestComponent()->OnQuestNotify.AddDynamic(this, &ThisClass::ShowCurrentQuestState);
    Player->GetQuestComponent()->OnSelectQuest.AddDynamic(this, &ThisClass::EditActiveQuest);
    Player->GetQuestComponent()->OnQuestStepUpdate.AddDynamic(this, &ThisClass::EditActiveQuestStep);
    Player->GetQuestComponent()->OnCompleteQuest.AddDynamic(this, &ThisClass::HideActiveQuest);
    Player->GetQuestComponent()->SetTTSOnAddedCompletedQuest.AddDynamic(this, &ThisClass::SpeakQuestTTS);
}

void UQuestWidget::ShowActiveQuest()
{
    ShowActiveQuestUpdate();
}

void UQuestWidget::ShowCurrentQuestState(bool bCompleted, FString QuestName)
{
    ShowCurrentQuestStateUpdate(bCompleted, QuestName);
}

void UQuestWidget::EditActiveQuest(FString QuestName, FString QuestDesc)
{
    EditActiveQuestUpdate(QuestName, QuestDesc);
}

void UQuestWidget::EditActiveQuestStep()
{
    EditActiveQuestStepUpdate();
}

void UQuestWidget::HideActiveQuest()
{
    HideActiveQuestUpdate();
}

void UQuestWidget::SpeakQuestTTS(TArray<FString> TTSText)
{
    SpeakQuestTTSUpdate(TTSText);
}
