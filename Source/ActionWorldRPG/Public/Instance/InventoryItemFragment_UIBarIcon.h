// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instance/InventoryItemDefinition.h"
#include "Styling/SlateBrush.h"
#include "InventoryItemFragment_UIBarIcon.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UInventoryItemFragment_UIBarIcon : public UInventoryItemFragment
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FSlateBrush Brush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FSlateBrush AmmoBrush;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FText DisplayNameWhenEquipped;
};
