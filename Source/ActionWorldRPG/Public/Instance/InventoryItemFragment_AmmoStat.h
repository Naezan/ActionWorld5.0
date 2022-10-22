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
	//ó�� ������ �Ѿ� ������ �����մϴ�.
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TMap<FGameplayTag, int32> DefaultAmmoStats;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;

	//���� ������ 0�� ��ȯ�մϴ�.
	int32 GetItemValueByTag(FGameplayTag Tag) const;
};
