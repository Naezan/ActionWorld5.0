// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Quest/QuestTriggerBase.h"
#include "AbilitySystemInterface.h"
#include "QuestTriggerItem.generated.h"

/**
 *
 */
 //BP에서 InteractInterface상속
UCLASS(BlueprintType, Blueprintable)//블루프린트의 변수로 사용, 블루프린트로 생성가능
class ACTIONWORLDRPG_API AQuestTriggerItem : public AQuestTriggerBase, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AQuestTriggerItem();

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//Interact인터페이스의 오버라이딩함수에서 호출됩니다.
	UFUNCTION(BlueprintCallable)
		void InteractQuestItem();

protected:
	UPROPERTY()
		class UActionAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> MonsterSpawners;
};
