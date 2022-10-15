// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/Quest/QuestData.h"
#include "QuestSystemComponent.generated.h"

//퀘스트가 추가되거나 삭제될때 추가된 UI함수를 브로드캐스팅합니다.
//HUD의 오른쪽에 현재 퀘스트의 이름 및 내용을 짧게 표시해줍니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAddedRemovedQuest);

//퀘스트가 완료되거나 받았을때(추가) 플레이어에게 알려줄 UI함수를 브로드캐스팅합니다.
//전면에 큰 UI로 ??퀘스트완료나 ??퀘스트 + 퀘스트내용을 출력합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNotifyQuest, bool, bQuestCompleted, FString, QuestName);

//퀘스트가 선택될 때 추가된 UI함수를 브로드캐스팅합니다.
//HUD의 오른쪽에 현재 퀘스트 표시를 수정합니다?
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectQuest, FString, QuestName, FString, QuestDesc);

//퀘스트가 완료스탭이 완료될 때 추가된 UI함수를 브로드캐스팅합니다.
//HUD의 오른쪽에 현재 퀘스트의 디테일내용을 수정합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompleteQuestStepUpdate);

//퀘스트가 완료될 때 추가된 UI함수를 브로드캐스팅합니다.
//HUD의 오른쪽에 현재 퀘스트 표시를 삭제합니다.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompleteQuest);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetTTSOnAddedOrCompletedQuest, TArray<FString>, TTSText);

UCLASS()
class ACTIONWORLDRPG_API UQuestSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestSystemComponent();

	/** Used to update variables */
	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "Event Dispatchers")
		FAddedRemovedQuest OnAddedRemovedQuest;
	/** Used for UI to notify the player of quest changes */
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FNotifyQuest OnQuestNotify;
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FSelectQuest OnSelectQuest;
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FCompleteQuestStepUpdate OnQuestStepUpdate;
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FCompleteQuest OnCompleteQuest;
	//TTS용 델리게이트입니다.
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FSetTTSOnAddedOrCompletedQuest SetTTSOnAddedCompletedQuest;

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
	//현재 게임에선 사용하지 않습니다.
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
