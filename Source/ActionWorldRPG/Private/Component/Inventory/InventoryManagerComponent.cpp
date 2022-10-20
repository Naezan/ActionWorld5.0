// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryManagerComponent.h"
#include "Instance/InventoryItemInstance.h"
#include "Instance/InventoryItemDefinition.h"

UInventoryManagerComponent::UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)//�κ��丮 ����Ʈ�� ���� ������Ʈ�� �κ��丮 �Ŵ���
{

}

UInventoryItemInstance* FInventoryList::AddItem(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//������ �ӽð�? �� ���� ������
	UInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	//�÷��̾�
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	//������ ����� �߰�
	FInventoryItemInfo& NewItem = Items.AddDefaulted_GetRef();
	//�������� �ӽ� �ν��Ͻ� ����
	NewItem.Instance = NewObject<UInventoryItemInstance>(OwningActor);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	//�ӽ� �ν��Ͻ��� ������ ���� �߰�
	NewItem.Instance->SetItemDef(ItemDef);
	//������ Fragment�� �߰��� �ΰ���Ұ� �ִٸ� �߰�
	for (UInventoryItemFragment* Fragment : GetDefault<UInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewItem.Instance);
		}
	}
	NewItem.StackCount = StackCount;
	Result = NewItem.Instance;

	//�߰��� �����۰��� ������ �����ͼ� �� �� ����
	return Result;
}

void FInventoryList::AddItem(UInventoryItemInstance* Instance)
{
	//�ƹ��͵� ���� �ʽ��ϴ�.
	//unimplemented();
}

void FInventoryList::RemoveItem(UInventoryItemInstance* Instance)
{
	for (auto ItemIt = Items.CreateIterator(); ItemIt; ++ItemIt)
	{
		FInventoryItemInfo& Item = *ItemIt;
		if (Item.Instance == Instance)
		{
			ItemIt.RemoveCurrent();
		}
	}
}

TArray<UInventoryItemInstance*> FInventoryList::GetAllItems() const
{
	TArray<UInventoryItemInstance*> Results;
	//���� Ȯ��
	Results.Reserve(Items.Num());
	for (const FInventoryItemInfo& Entry : Items)
	{
		if (Entry.Instance != nullptr)
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

bool UInventoryManagerComponent::CanAddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//@TODO: Add support for stack limit / uniqueness checks / etc...
	return true;
}

UInventoryItemInstance* UInventoryManagerComponent::AddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount)
{
	UInventoryItemInstance* Result = nullptr;
	if (ItemDef != nullptr)
	{
		Result = InventoryList.AddItem(ItemDef, StackCount);
	}
	return Result;
}

void UInventoryManagerComponent::AddItemInstance(UInventoryItemInstance* ItemInstance)
{
	InventoryList.AddItem(ItemInstance);
}

void UInventoryManagerComponent::RemoveItemInstance(UInventoryItemInstance* ItemInstance)
{
	InventoryList.RemoveItem(ItemInstance);
}

TArray<UInventoryItemInstance*> UInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

UInventoryItemInstance* UInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const
{
	for (const FInventoryItemInfo& Entry : InventoryList.Items)
	{
		UInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FInventoryItemInfo& Entry : InventoryList.Items)
	{
		UInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef() == ItemDef)
			{
				++TotalCount;
			}
		}
	}

	return TotalCount;
}

bool UInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UInventoryItemInstance* Instance = UInventoryManagerComponent::FindFirstItemStackByDefinition(ItemDef))
		{
			InventoryList.RemoveItem(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}