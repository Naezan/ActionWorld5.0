// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Instance/InventoryItemDefinition.h"
#include "InventoryItemFragment_WorldIcon.generated.h"

class USkeletalMesh;

UCLASS()
class ACTIONWORLDRPG_API UInventoryItemFragment_WorldIcon : public UInventoryItemFragment
{
	GENERATED_BODY()
	
public:
	UInventoryItemFragment_WorldIcon();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		TObjectPtr<USkeletalMesh> SkeletalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Appearance)
		FLinearColor PadColor;
};
