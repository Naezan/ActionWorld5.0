// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InventoryItem.h"

bool UInventoryItem::IsConsumable() const
{
	if (MaxCount <= 0)
	{
		return true;
	}

    return false;
}

FPrimaryAssetId UInventoryItem::GetPrimaryAssetId() const
{
	//이름은 언제사용하는지 모르겠다
	return FPrimaryAssetId(ItemType, GetFName());
}
