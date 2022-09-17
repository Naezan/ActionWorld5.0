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
