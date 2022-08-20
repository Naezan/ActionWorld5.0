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
struct FEquipItemContainer;
typedef TMulticastDelegate<void(bool, UInventoryItem*)> FOnInventoryItemChangedNative;
typedef TMulticastDelegate<void(FEquipItemContainer, UInventoryItem*)> FOnEquippedItemChangedNative;
typedef TMulticastDelegate<void()> FOnInventoryLoadedNative;

/**
 * 
 */
class ACTIONWORLDRPG_API IInventoryInterface
{
	GENERATED_BODY()

public:
	//모든 인벤토리가 담긴 맵을가져옵니다.
	virtual const TMap<UInventoryItem*, FItemData>& GetInventoryDataMap() const = 0;

	//모든 장착된 아이템이 담긴 맵을 가져옵니다.
	virtual const TMap<FEquipItemContainer, UInventoryItem*>& GetEquippedItemMap() const = 0;

	//C++에서만 쓸 수 있는 인벤토리 아이템 델리게이트를 가져옵니다.
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	//C++에서만 쓸 수 있는 장착된 아이템 델리게이트를 가져옵니다.
	virtual FOnEquippedItemChangedNative& GetEquippedItemChangedDelegate() = 0;

	//C++에서만 쓸 수 있는 인벤토리 로드 델리게이트를 가져옵니다.
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
