// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Quest/QuestData.h"

FReward::FReward()
{
	bIsEmpty = false;
}

void FReward::Clear()
{
	RewardData.Empty();
	NextQuest = nullptr;
	bIsEmpty = true;
}

bool FReward::IsEmpty()
{
	return bIsEmpty;
}


FQuest::FQuest()
{
	bIsEmpty = false;
}

void FQuest::Clear()
{
	QuestType = EQuestType::QT_None;
	QuestName = "";
	QeustDescription = "";
	TargetClass = nullptr;
	TargetCount = 0;
	CompleteTargetClass = nullptr;
	Reward = FReward();
	bActive = false;
	bIsEmpty = true;
	bComplete = false;
}

bool FQuest::IsEmpty()
{
	return bIsEmpty;
}