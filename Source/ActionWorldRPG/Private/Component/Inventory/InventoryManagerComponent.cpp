// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryManagerComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	//인벤토리 초기화 하드코딩 총 슬롯은3개
	EquippedItems.Add(FEquipItemContainer(0), nullptr);
	EquippedItems.Add(FEquipItemContainer(1), nullptr);
	EquippedItems.Add(FEquipItemContainer(2), nullptr);
}

void UInventoryManagerComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent BeginPlay"));
}

bool UInventoryManagerComponent::AddInventoryItem(UInventoryItem* NewItem, int32 ItemCount, bool bAutoEquip)
{
	bool bChanged = false;

	if (!NewItem)
	{
		//아이템을 넣지 않은 경우
		return false;
	}

	if (ItemCount <= 0)
	{
		//아이템 갯수가 잘못 입력된 경우
		return false;
	}

	//인벤토리에 기존의 아이템이 있는지 검사합니다.
	FItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	//기존의 아이템이 있다면 기존의 아이템에 갯수를 더해줍니다.
	//기존의 아이템이 없다면 추가하고싶은 ItemCount만큼만 더해줍니다. *아직 추가작업은 하지 않았습니다.
	FItemData NewData = OldData;
	NewData.UpdateItemData(FItemData(ItemCount), NewItem->MaxCount);

	if (OldData != NewData)
	{
		InventoryData.Add(NewItem, NewData);

		//인벤토리 아이템 추가 바인드 함수호출 등
		NotifyInventoryItemChanged(true, NewItem);

		bChanged = true;
	}

	if (bAutoEquip)
	{
		//인덱스가 제일 낮은위치에 장착합니다.
		bChanged |= FillEmptyEquippedItem(NewItem);
	}

	return bChanged;
}

bool UInventoryManagerComponent::RemoveInventoryItem(UInventoryItem* RemoveItem, int32 RemoveItemCount)
{
	if (!RemoveItem)
	{
		//아이템을 넣지 않은 경우
		return false;
	}

	FItemData NewData;
	GetInventoryItemData(RemoveItem, NewData);

	if (!NewData.IsValid())
	{
		//아이템을 찾지 못했습니다.
		return false;
	}

	//0보다 작은 값을 입력한 경우 아이템을 전부 삭제합니다.
	if (RemoveItemCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveItemCount;
	}

	if (NewData.ItemCount > 0)
	{
		//아이템을 업데이트 합니다.
		InventoryData.Add(RemoveItem, NewData);
	}
	else
	{
		//갯수가 0이 됬으므로 아이템을 삭제합니다.
		InventoryData.Remove(RemoveItem);

		//장비창에 있다면 그 값을 비웁니다.
		for (TPair<FEquipItemContainer, UInventoryItem*>& EquipItem : EquippedItems)
		{
			if (EquipItem.Value == RemoveItem)
			{
				EquipItem.Value = nullptr;
				NotifyEquippedItemChanged(EquipItem.Key, EquipItem.Value);
			}
		}
	}

	//인벤토리 아이템 제거 바인드 함수호출 등
	NotifyInventoryItemChanged(false, RemoveItem);

	return true;
}

void UInventoryManagerComponent::GetInventoryItemsForType(TArray<UInventoryItem*>& Items, FPrimaryAssetType ItemType)
{
	for (TPair<UInventoryItem*, FItemData>& Item : InventoryData)
	{
		if (Item.Key)
		{
			FPrimaryAssetId AssetId = Item.Key->GetPrimaryAssetId();

			//에셋 타입을 설정하지 않았다면 모두 찾아옵니다.
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Item.Key);
			}
		}
	}
}

int32 UInventoryManagerComponent::GetInventoryItemCount(UInventoryItem* Item) const
{
	const FItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem != nullptr)
	{
		return FoundItem->ItemCount;
	}
	return 0;
}

bool UInventoryManagerComponent::GetInventoryItemData(UInventoryItem* Item, FItemData& ItemData) const
{
	const FItemData* FoundItem = InventoryData.Find(Item);

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}

	//값을 못찾을 경우 아이템갯수를 0으로 입력해 에러를 반환하도록 유도
	ItemData = FItemData(0);
	return false;
}

bool UInventoryManagerComponent::SetEquippedItem(FEquipItemContainer EquipItemType, UInventoryItem* NewItem)
{
	bool bFound = false;
	for (TPair<FEquipItemContainer, UInventoryItem*>& Item : EquippedItems)
	{
		//아이템을 새로 넣어줍니다(바꿔줍니다).
		if (Item.Key == EquipItemType)
		{
			Item.Value = NewItem;
			NotifyEquippedItemChanged(Item.Key, Item.Value);

			bFound = true;
		}
		//같은 아이템을 없애줍니다.
		else if (NewItem != nullptr && Item.Value == NewItem)
		{
			Item.Value = nullptr;
			NotifyEquippedItemChanged(Item.Key, Item.Value);
		}
	}

	return bFound;
}

UInventoryItem* UInventoryManagerComponent::GetEquippedItem(FEquipItemContainer EquipItemType) const
{
	UInventoryItem* const* FoundEquipItem = EquippedItems.Find(EquipItemType);

	if (FoundEquipItem != nullptr)
	{
		return *FoundEquipItem;
	}
	return nullptr;
}

void UInventoryManagerComponent::GetAllEquippedItems(TArray<UInventoryItem*>& Items)
{
	for (TPair<FEquipItemContainer, UInventoryItem*>& Item : EquippedItems)
	{
		Items.Add(Item.Value);
	}
}

void UInventoryManagerComponent::FillEmptyEquippedItems()
{
	for (const TPair<UInventoryItem*, FItemData>& Item : InventoryData)
	{
		FillEmptyEquippedItem(Item.Key);
	}
}

bool UInventoryManagerComponent::FillEmptyEquippedItem(UInventoryItem* NewItem)
{
	//아이템의 데이터 에셋 정보를 가져옵니다.
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FEquipItemContainer EmptyItemContainer;
	for (TPair<FEquipItemContainer, UInventoryItem*>& EquipItem : EquippedItems)
	{
		if (EquipItem.Value == NewItem)
		{
			//이미 같은 아이템이 있는 경우
			return false;
		}
		//아이템종류칸은 있지만 아이템은 없는 경우 + 인덱스값이 작은값에 되도록이면 넣도록
		//(상황에 맞게 인덱스 위치를 고정하고 셋팅할 수도 있습니다.)
		else if (EquipItem.Value == nullptr &&
			(!EmptyItemContainer.IsValid() || EmptyItemContainer.EquipItemIndex > EquipItem.Key.EquipItemIndex))
		{
			EmptyItemContainer = EquipItem.Key;
		}
	}

	if (EmptyItemContainer.IsValid())
	{
		EquippedItems[EmptyItemContainer] = NewItem;
		NotifyEquippedItemChanged(EmptyItemContainer, NewItem);
		return true;
	}

	return false;
}

void UInventoryManagerComponent::NotifyInventoryItemChanged(bool bAdded, UInventoryItem* Item)
{
	//BP델리게이트 호출 이전에 C++델리게이트를 먼저 호출합니다.
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	//BP함수를 호출해줍니다. 현재 컴포넌트는 게임피쳐에서 사용되고 있기 때문에 BP함수가 호출되지 않습니다.
	InventoryItemChanged(bAdded, Item);
}

void UInventoryManagerComponent::NotifyEquippedItemChanged(FEquipItemContainer ItemType, UInventoryItem* Item)
{
	//BP델리게이트 호출 이전에 C++델리게이트를 먼저 호출합니다.
	OnEquippedItemChangedNative.Broadcast(ItemType, Item);
	OnEquippedItemChanged.Broadcast(ItemType, Item);

	//현재 컴포넌트는 게임피쳐에서 사용되고 있기 때문에 BP함수가 호출되지 않습니다.
	EquippedItemChanged(ItemType, Item);
}

void UInventoryManagerComponent::NotifyInventoryChanged()
{
	//C++델리게이트 먼저 호출
	OnInventoryLoadedNative.Broadcast();
	//BP델리게이트 호출
	OnInventoryLoaded.Broadcast();
}
