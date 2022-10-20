// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/EquipmentBarComponent.h"
#include "Instance/InventoryItemInstance.h"
#include "Instance/InventoryItemDefinition.h"
#include "Instance/EquipmentDefinition.h"
#include "Instance/EquipmentInstance.h"
#include "Component/Inventory/EquipmentManagerComponent.h"
#include "Instance/InventoryItemFragment_EquipItem.h"

UEquipmentBarComponent::UEquipmentBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UEquipmentBarComponent::BeginPlay()
{
	//??기본슬롯을 미리 생성?
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UEquipmentBarComponent::CycleActiveSlotForward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 EndIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != EndIndex);
}

void UEquipmentBarComponent::CycleActiveSlotBackward()
{
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 EndIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;
	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != EndIndex);
}

void UEquipmentBarComponent::SetActiveSlotIndex(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		UnequipItemInSlot();

		ActiveSlotIndex = NewIndex;

		EquipItemInSlot();
	}
}

UInventoryItemInstance* UEquipmentBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UEquipmentBarComponent::GetNextFreeItemSlot() const
{
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		if (Slots[i] == nullptr)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

void UEquipmentBarComponent::AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

UInventoryItemInstance* UEquipmentBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	//현재 인덱스의 무기 장착해제
	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		//일단 장비해제 비무장
		ActiveSlotIndex = -1;
	}

	UInventoryItemInstance* Result = nullptr;

	//제거할인덱스가 유효하다면
	if (Slots.IsValidIndex(SlotIndex))
	{
		//인덱스에 있는 장비가 있든 없든 할당
		Result = Slots[SlotIndex];

		//장비가 있다면 제거
		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
		}
	}

	//무조건 null리턴일거 같은데?
	return Result;
}

void UEquipmentBarComponent::UnequipItemInSlot()
{
	if (UEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->UnequipItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UEquipmentBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		//생성된 인벤토리 아이템 인스턴스에서 인벤토리 아이템의 정보가담긴 Definition을 가져온다
		//그 ItemDefinition에서 무기만을 담고있는 정보를 가져온다.
		if (const UInventoryItemFragment_EquipItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryItemFragment_EquipItem>())
		{
			TSubclassOf<UEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);

					//인벤토리인스턴스를 장비인스턴스의 주인?으로 설정
					if (EquippedItem != nullptr)
					{
						EquippedItem->SetInstigator(SlotItem);
					}
				}
			}
		}
	}
}

UEquipmentManagerComponent* UEquipmentBarComponent::FindEquipmentManager() const
{
	if (AController* OwnerController = Cast<AController>(GetOwner()))
	{
		if (APawn* Pawn = OwnerController->GetPawn())
		{
			return Pawn->FindComponentByClass<UEquipmentManagerComponent>();
		}
	}
	return nullptr;
}