// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instance/InventoryItemDefinition.h"
#include "Instance/EquipmentDefinition.h"
#include "InventoryItemFragment_EquipItem.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UInventoryItemFragment_EquipItem : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		TSubclassOf<UEquipmentDefinition> EquipmentDefinition;
};
