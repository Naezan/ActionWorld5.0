// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instance/InventoryItemDefinition.h"
#include "GameplayTagContainer.h"
#include "InventoryItemFragment_AmmoStat.generated.h"

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API UInventoryItemFragment_AmmoStat : public UInventoryItemFragment
{
	GENERATED_BODY()


protected:
	//처음 셋팅할 총알 정보를 저장합니다.
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TMap<FGameplayTag, int32> DefaultAmmoStats;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;

	//값이 없으면 0을 반환합니다.
	int32 GetItemValueByTag(FGameplayTag Tag) const;
};
