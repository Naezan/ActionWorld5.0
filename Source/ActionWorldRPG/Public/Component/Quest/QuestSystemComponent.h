// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/Quest/QuestData.h"
#include "QuestSystemComponent.generated.h"


UCLASS()
class ACTIONWORLDRPG_API UQuestSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestSystemComponent();

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	//퀘스트를 추가하면 추가한퀘스트가 현재 퀘스트가 됩니다.
	UFUNCTION(BlueprintCallable)
		bool AddQuest(FQuest Quest);
	//현재활성화퀘스트를 완료합니다.
	UFUNCTION(BlueprintCallable)
		void CompleteQuest(bool& bSuccess);
	//주로몬스터가 죽을때 이 함수를 호출합니다. 트리거의 Complete함수를 대체합니다.
	UFUNCTION(BlueprintCallable)
		void CompleteQuestStep(bool& bSuccess, TSubclassOf<AActor> TargetActorClass);
	//퀘스트를 선택해서 현재퀘스트로 셋팅합니다.
	UFUNCTION(BlueprintCallable)
		bool SelectQuest(const int32 QuestID);
	//인덱스에맞는퀘스트를지우고 그 퀘스트가 현재퀘스트라면 새로운퀘스트를 선택합니다.
	UFUNCTION(BlueprintCallable)
		bool RemoveQuest(const int32 QuestID);

	UFUNCTION(BlueprintCallable)
		void ResetCurrentQuest();

	UFUNCTION(BlueprintPure)
		FQuest GetActiveQuest() const { return ActiveQuest; }

	UFUNCTION(BlueprintPure)
		UQuestData* GetQuests() const { return Quests; }

	UFUNCTION(BlueprintPure)
		bool HasCurActiveQuestID() const { return CurrentQuestID != -1; }

	UFUNCTION(BlueprintPure)
		int32 GetCurActiveQuestID() const { return CurrentQuestID; }


private:
	//현재 활성화된 퀘스트
	UPROPERTY(VisibleAnywhere)
		FQuest ActiveQuest;

	/*
	플레이어가 현재 수주한 모든 퀘스트가 들어있는 데이터
	퀘스트가 완료되면 완료된 Active퀘스트와 일치하는 인덱스를 제거하는방식
	*/
	UQuestData* Quests;

	//현재 퀘스트의 인덱스
	int32 CurrentQuestID = -1;

	//게임 인스턴스 세이브
};
