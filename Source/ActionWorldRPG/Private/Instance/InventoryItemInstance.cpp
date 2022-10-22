// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/InventoryItemInstance.h"
#include "Instance/InventoryItemDefinition.h"

UInventoryItemInstance::UInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void FGameplayTagStackList::AddStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		for (FGameplayTagStack& Stack : Stacks)
		{
			if (Stack.Tag == Tag)
			{
				const int32 NewCount = Stack.StackCount + StackCount;
				Stack.StackCount = NewCount;
				return;
			}
		}

		//ã�� ���������
		//Emplace�� ���ؼ� ��Ҹ� �����ϰ� �߰�
		Stacks.Emplace(Tag, StackCount);
	}
}

void FGameplayTagStackList::RemoveStack(FGameplayTag Tag, int32 StackCount)
{
	if (!Tag.IsValid())
	{
		return;
	}

	if (StackCount > 0)
	{
		for (auto It = Stacks.CreateIterator(); It; ++It)
		{
			FGameplayTagStack& Stack = *It;
			if (Stack.Tag == Tag)
			{
				//������ ī��Ʈ�� ���� �� ���ٸ� ������ ����
				if (Stack.StackCount <= StackCount)
				{
					It.RemoveCurrent();
				}
				else
				{
					const int32 NewCount = Stack.StackCount - StackCount;
					Stack.StackCount = NewCount;
				}
				return;
			}
		}
	}
}

int32 FGameplayTagStackList::GetStackCount(FGameplayTag Tag) const
{
	for (const FGameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == Tag)
		{
			return Stack.StackCount;
		}
	}

	return 0;
}

bool FGameplayTagStackList::ContainsTag(FGameplayTag Tag) const
{
	for (const FGameplayTagStack& Stack : Stacks)
	{
		if (Stack.Tag == Tag)
		{
			return true;
		}
	}

	return false;
}

void UInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.AddStack(Tag, StackCount);
}

void UInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 UInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool UInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void UInventoryItemInstance::SetItemDef(TSubclassOf<UInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

const UInventoryItemFragment* UInventoryItemInstance::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		//�ӽ��� ItemDef�� �ν��Ͻ��� ���� �´� FragmentClass�� ã���ϴ�. �ӽ÷� ����ϴ� ������ ����ֽ��ϴ�.
		return GetDefault<UInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}