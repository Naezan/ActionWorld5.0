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
