// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/InventoryManagerComponent.h"
#include "Instance/InventoryItemInstance.h"
#include "Instance/InventoryItemDefinition.h"

UInventoryManagerComponent::UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)//인벤토리 리스트의 소유 컴포는트는 인벤토리 매니저
{

}

UInventoryItemInstance* FInventoryList::AddItem(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount)
{
	//생성된 임시값? 을 가질 포인터
	UInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	//플레이어
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	//아이템 빈공간 추가
	FInventoryItemInfo& NewItem = Items.AddDefaulted_GetRef();
	//아이템의 임시 인스턴스 생성
	NewItem.Instance = NewObject<UInventoryItemInstance>(OwningActor);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	//임시 인스턴스에 아이템 정보 추가
	NewItem.Instance->SetItemDef(ItemDef);
	//생성후 Fragment에 추가할 부가요소가 있다면 추가
	for (UInventoryItemFragment* Fragment : GetDefault<UInventoryItemDefinition>(ItemDef)->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(NewItem.Instance);
		}
	}
	NewItem.StackCount = StackCount;
	Result = NewItem.Instance;

	//추가된 아이템값의 정보를 가져와서 쓸 수 있음
	return Result;
}

void FInventoryList::AddItem(UInventoryItemInstance* Instance)
{
	//아무것도 하지 않습니다.
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
	//공간 확보
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