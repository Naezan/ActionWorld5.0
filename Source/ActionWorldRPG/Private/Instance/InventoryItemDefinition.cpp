// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/InventoryItemDefinition.h"

UInventoryItemDefinition::UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInventoryItemFragment* UInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UInventoryItemFragment* Fragment : Fragments)
		{
			//����Ÿ���� Ŭ������ ã���ϴ�.
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}

const UInventoryItemFragment* UInventoryFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UInventoryItemDefinition> ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		//�ӽ��� ItemDef�� �ν��Ͻ��� ���� �´� FragmentClass�� ã���ϴ�. �ӽ÷� ����ϴ� ������ ����ֽ��ϴ�.
		return GetDefault<UInventoryItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
