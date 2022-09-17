// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Quest/QuestTriggerBase.h"
#include "AbilitySystemInterface.h"
#include "QuestTriggerItem.generated.h"

/**
 *
 */
 //BP���� InteractInterface���
UCLASS(BlueprintType, Blueprintable)//�������Ʈ�� ������ ���, �������Ʈ�� ��������
class ACTIONWORLDRPG_API AQuestTriggerItem : public AQuestTriggerBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AQuestTriggerItem();

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//Interact�������̽��� �������̵��Լ����� ȣ��˴ϴ�.
	UFUNCTION(BlueprintCallable)
		void InteractQuestItem();

protected:
	UPROPERTY()
		class UActionAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> MonsterSpawners;
};
