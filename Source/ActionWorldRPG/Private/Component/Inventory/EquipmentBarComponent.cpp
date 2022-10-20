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
	//??�⺻������ �̸� ����?
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
	//���� �ε����� ���� ��������
	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
		//�ϴ� ������� ����
		ActiveSlotIndex = -1;
	}

	UInventoryItemInstance* Result = nullptr;

	//�������ε����� ��ȿ�ϴٸ�
	if (Slots.IsValidIndex(SlotIndex))
	{
		//�ε����� �ִ� ��� �ֵ� ���� �Ҵ�
		Result = Slots[SlotIndex];

		//��� �ִٸ� ����
		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
		}
	}

	//������ null�����ϰ� ������?
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
		//������ �κ��丮 ������ �ν��Ͻ����� �κ��丮 �������� ��������� Definition�� �����´�
		//�� ItemDefinition���� ���⸸�� ����ִ� ������ �����´�.
		if (const UInventoryItemFragment_EquipItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryItemFragment_EquipItem>())
		{
			TSubclassOf<UEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->EquipItem(EquipDef);

					//�κ��丮�ν��Ͻ��� ����ν��Ͻ��� ����?���� ����
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