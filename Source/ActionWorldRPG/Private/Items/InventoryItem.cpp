// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InventoryItem.h"
#include "GameplayEffect.h"

int32 UInventoryItem::GetItemCount() const
{
	float Count = 1;
	if (OwnGameplayEffect != nullptr)
	{	
		//하드코딩
		if (!OwnGameplayEffect.GetDefaultObject()->Modifiers.IsEmpty())
		{
			OwnGameplayEffect.GetDefaultObject()->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Count);
		}
	}
	return Count;
}

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
