// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Component/Quest/QuestData.h"
#include "QuestSystemComponent.generated.h"

//����Ʈ�� �߰��ǰų� �����ɶ� �߰��� UI�Լ��� ��ε�ĳ�����մϴ�.
//HUD�� �����ʿ� ���� ����Ʈ�� �̸� �� ������ ª�� ǥ�����ݴϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAddedRemovedQuest);

//����Ʈ�� �Ϸ�ǰų� �޾�����(�߰�) �÷��̾�� �˷��� UI�Լ��� ��ε�ĳ�����մϴ�.
//���鿡 ū UI�� ??����Ʈ�Ϸᳪ ??����Ʈ + ����Ʈ������ ����մϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNotifyQuest, bool, bQuestCompleted, FString, QuestName);

//����Ʈ�� ���õ� �� �߰��� UI�Լ��� ��ε�ĳ�����մϴ�.
//HUD�� �����ʿ� ���� ����Ʈ ǥ�ø� �����մϴ�?
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSelectQuest, FString, QuestName, FString, QuestDesc);

//����Ʈ�� �Ϸὺ���� �Ϸ�� �� �߰��� UI�Լ��� ��ε�ĳ�����մϴ�.
//HUD�� �����ʿ� ���� ����Ʈ�� �����ϳ����� �����մϴ�.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCompleteQuestStepUpdate);

//����Ʈ�� �Ϸ�� �� �߰��� UI�Լ��� ��ε�ĳ�����մϴ�.
//HUD�� �����ʿ� ���� ����Ʈ ǥ�ø� �����մϴ�.
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
	//TTS�� ��������Ʈ�Դϴ�.
	UPROPERTY(BlueprintAssignable, Category = "Event Dispatchers")
		FSetTTSOnAddedOrCompletedQuest SetTTSOnAddedCompletedQuest;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


public:
	//����Ʈ�� �߰��ϸ� �߰�������Ʈ�� ���� ����Ʈ�� �˴ϴ�.
	UFUNCTION(BlueprintCallable)
		bool AddQuest(FQuest Quest);
	//����Ȱ��ȭ����Ʈ�� �Ϸ��մϴ�.
	UFUNCTION(BlueprintCallable)
		void CompleteQuest(bool& bSuccess);
	//�ַθ��Ͱ� ������ �� �Լ��� ȣ���մϴ�. Ʈ������ Complete�Լ��� ��ü�մϴ�.
	UFUNCTION(BlueprintCallable)
		void CompleteQuestStep(bool& bSuccess, TSubclassOf<AActor> TargetActorClass);
	//����Ʈ�� �����ؼ� ��������Ʈ�� �����մϴ�.
	UFUNCTION(BlueprintCallable)
		bool SelectQuest(const int32 QuestID);
	//�ε������´�����Ʈ������� �� ����Ʈ�� ��������Ʈ��� ���ο�����Ʈ�� �����մϴ�.
	//���� ���ӿ��� ������� �ʽ��ϴ�.
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
	//���� Ȱ��ȭ�� ����Ʈ
	UPROPERTY(VisibleAnywhere)
		FQuest ActiveQuest;

	/*
	�÷��̾ ���� ������ ��� ����Ʈ�� ����ִ� ������
	����Ʈ�� �Ϸ�Ǹ� �Ϸ�� Active����Ʈ�� ��ġ�ϴ� �ε����� �����ϴ¹��
	*/
	UQuestData* Quests;

	//���� ����Ʈ�� �ε���
	int32 CurrentQuestID = -1;

	//���� �ν��Ͻ� ���̺�
};
