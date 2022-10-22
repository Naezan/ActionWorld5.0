// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/InventoryItemFragment_AmmoStat.h"
#include "Instance/InventoryItemInstance.h"

void UInventoryItemFragment_AmmoStat::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	for (const TPair<FGameplayTag, int32>& Ammo : DefaultAmmoStats)
	{
		Instance->AddStatTagStack(Ammo.Key, Ammo.Value);
	}
}

int32 UInventoryItemFragment_AmmoStat::GetItemValueByTag(FGameplayTag Tag) const
{
	if (const int32* value = DefaultAmmoStats.Find(Tag))
	{
		return *value;
	}

	return 0;
}
