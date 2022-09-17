// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryManagerComponent.h"

UInventoryManagerComponent::UInventoryManagerComponent()
{
	//�κ��丮 �ʱ�ȭ �ϵ��ڵ� �� ������3��
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
		//�������� ���� ���� ���
		return false;
	}

	if (ItemCount <= 0)
	{
		//������ ������ �߸� �Էµ� ���
		return false;
	}

	//�κ��丮�� ������ �������� �ִ��� �˻��մϴ�.
	FItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	//������ �������� �ִٸ� ������ �����ۿ� ������ �����ݴϴ�.
	//������ �������� ���ٸ� �߰��ϰ���� ItemCount��ŭ�� �����ݴϴ�. *���� �߰��۾��� ���� �ʾҽ��ϴ�.
	FItemData NewData = OldData;
	NewData.UpdateItemData(FItemData(ItemCount), NewItem->MaxCount);

	if (OldData != NewData)
	{
		InventoryData.Add(NewItem, NewData);

		//�κ��丮 ������ �߰� ���ε� �Լ�ȣ�� ��
		NotifyInventoryItemChanged(true, NewItem);

		bChanged = true;
	}

	if (bAutoEquip)
	{
		//�ε����� ���� ������ġ�� �����մϴ�.
		bChanged |= FillEmptyEquippedItem(NewItem);
	}

	return bChanged;
}

bool UInventoryManagerComponent::RemoveInventoryItem(UInventoryItem* RemoveItem, int32 RemoveItemCount)
{
	if (!RemoveItem)
	{
		//�������� ���� ���� ���
		return false;
	}

	FItemData NewData;
	GetInventoryItemData(RemoveItem, NewData);

	if (!NewData.IsValid())
	{
		//�������� ã�� ���߽��ϴ�.
		return false;
	}

	//0���� ���� ���� �Է��� ��� �������� ���� �����մϴ�.
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
		//�������� ������Ʈ �մϴ�.
		InventoryData.Add(RemoveItem, NewData);
	}
	else
	{
		//������ 0�� �����Ƿ� �������� �����մϴ�.
		InventoryData.Remove(RemoveItem);

		//���â�� �ִٸ� �� ���� ���ϴ�.
		for (TPair<FEquipItemContainer, UInventoryItem*>& EquipItem : EquippedItems)
		{
			if (EquipItem.Value == RemoveItem)
			{
				EquipItem.Value = nullptr;
				NotifyEquippedItemChanged(EquipItem.Key, EquipItem.Value);
			}
		}
	}

	//�κ��丮 ������ ���� ���ε� �Լ�ȣ�� ��
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

			//���� Ÿ���� �������� �ʾҴٸ� ��� ã�ƿɴϴ�.
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

	//���� ��ã�� ��� �����۰����� 0���� �Է��� ������ ��ȯ�ϵ��� ����
	ItemData = FItemData(0);
	return false;
}

bool UInventoryManagerComponent::SetEquippedItem(FEquipItemContainer EquipItemType, UInventoryItem* NewItem)
{
	bool bFound = false;
	for (TPair<FEquipItemContainer, UInventoryItem*>& Item : EquippedItems)
	{
		//�������� ���� �־��ݴϴ�(�ٲ��ݴϴ�).
		if (Item.Key == EquipItemType)
		{
			Item.Value = NewItem;
			NotifyEquippedItemChanged(Item.Key, Item.Value);

			bFound = true;
		}
		//���� �������� �����ݴϴ�.
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
	//�������� ������ ���� ������ �����ɴϴ�.
	FPrimaryAssetType NewItemType = NewItem->GetPrimaryAssetId().PrimaryAssetType;
	FEquipItemContainer EmptyItemContainer;
	for (TPair<FEquipItemContainer, UInventoryItem*>& EquipItem : EquippedItems)
	{
		if (EquipItem.Value == NewItem)
		{
			//�̹� ���� �������� �ִ� ���
			return false;
		}
		//����������ĭ�� ������ �������� ���� ��� + �ε������� �������� �ǵ����̸� �ֵ���
		//(��Ȳ�� �°� �ε��� ��ġ�� �����ϰ� ������ ���� �ֽ��ϴ�.)
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
	//BP��������Ʈ ȣ�� ������ C++��������Ʈ�� ���� ȣ���մϴ�.
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	//BP�Լ��� ȣ�����ݴϴ�. ���� ������Ʈ�� �������Ŀ��� ���ǰ� �ֱ� ������ BP�Լ��� ȣ����� �ʽ��ϴ�.
	InventoryItemChanged(bAdded, Item);
}

void UInventoryManagerComponent::NotifyEquippedItemChanged(FEquipItemContainer ItemType, UInventoryItem* Item)
{
	//BP��������Ʈ ȣ�� ������ C++��������Ʈ�� ���� ȣ���մϴ�.
	OnEquippedItemChangedNative.Broadcast(ItemType, Item);
	OnEquippedItemChanged.Broadcast(ItemType, Item);

	//���� ������Ʈ�� �������Ŀ��� ���ǰ� �ֱ� ������ BP�Լ��� ȣ����� �ʽ��ϴ�.
	EquippedItemChanged(ItemType, Item);
}

void UInventoryManagerComponent::NotifyInventoryChanged()
{
	//C++��������Ʈ ���� ȣ��
	OnInventoryLoadedNative.Broadcast();
	//BP��������Ʈ ȣ��
	OnInventoryLoaded.Broadcast();
}
