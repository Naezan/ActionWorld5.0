// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryManagerComponent.h"

bool UInventoryManagerComponent::AddInventoryItem(UInventoryItem* NewItem, int32 ItemCount, bool bAutoEquip)
{
	return false;
}

bool UInventoryManagerComponent::RemoveInventoryItem(UInventoryItem* RemoveItem, int32 RemoveItemCount)
{
	return false;
}

void UInventoryManagerComponent::GetInventoryItemsForType(TArray<UInventoryItem*>& Items, FPrimaryAssetType ItemType)
{
}

int32 UInventoryManagerComponent::GetInventoryItemCount(UInventoryItem* Item) const
{
	return int32();
}

void UInventoryManagerComponent::GetInventoryItemData(UInventoryItem* Item, FItemData& ItemData) const
{
}

bool UInventoryManagerComponent::SetEquippedItem(FEquipItemType EquipItemType, UInventoryItem* Item)
{
	return false;
}

UInventoryItem* UInventoryManagerComponent::GetEquippedItem(FEquipItemType EquipItemType) const
{
	return nullptr;
}

void UInventoryManagerComponent::GetAllEquippedItems(TArray<UInventoryItem*>& Items, FPrimaryAssetType ItemType)
{
}

void UInventoryManagerComponent::FillEmptyEquippedItems()
{
}
