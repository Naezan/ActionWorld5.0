// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InventoryInterface.generated.h"

// �������Ʈ���� �������̽��� ��ӹ��� �� ������ �մϴ�.
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
	//��� �κ��丮�� ��� ���������ɴϴ�.
	virtual const TMap<UInventoryItem*, FItemData>& GetInventoryDataMap() const = 0;

	//��� ������ �������� ��� ���� �����ɴϴ�.
	virtual const TMap<FEquipItemContainer, UInventoryItem*>& GetEquippedItemMap() const = 0;

	//C++������ �� �� �ִ� �κ��丮 ������ ��������Ʈ�� �����ɴϴ�.
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	//C++������ �� �� �ִ� ������ ������ ��������Ʈ�� �����ɴϴ�.
	virtual FOnEquippedItemChangedNative& GetEquippedItemChangedDelegate() = 0;

	//C++������ �� �� �ִ� �κ��丮 �ε� ��������Ʈ�� �����ɴϴ�.
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
