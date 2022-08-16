// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

// 블루프린트에서 인테퍼이스를 상속받을 수 없도록 합니다.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class UInventoryItem;
struct FItemData;
struct FEquipItemType;
typedef TMulticastDelegate<void(bool, UInventoryItem*)> FOnInventoryItemChangedNative;
typedef TMulticastDelegate<void(FEquipItemType, UInventoryItem*)> FOnEquippedItemChangedNative;
typedef TMulticastDelegate<void()> FOnInventoryLoadedNative;

/**
 * 
 */
class ACTIONWORLDRPG_API IInventoryInterface
{
	GENERATED_BODY()

public:
	/** Returns the map of items to data */
	virtual const TMap<UInventoryItem*, FItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	virtual const TMap<FEquipItemType, UInventoryItem*>& GetEquippedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	virtual FOnEquippedItemChangedNative& GetEquippedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
