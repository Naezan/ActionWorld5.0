// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InventoryItem.generated.h"

/**
 *
 */
UCLASS(BlueprintType)
class ACTIONWORLDRPG_API UInventoryItem : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UInventoryItem()
		: Price(0)
		, MaxCount(1)
	{}


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Item)
		FPrimaryAssetType ItemType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FText ItemDescription;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		FSlateBrush ItemIcon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Item)
		int32 Price;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Count)
		int32 MaxCount;

	//어떻게 사용하는거지?
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Count)
		bool IsConsumable() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
