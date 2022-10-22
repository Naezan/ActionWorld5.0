// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"

class UInventoryItemDefinition;

//źâ�� ������ �� ����մϴ�.
USTRUCT(BlueprintType)
struct FGameplayTagStack
{
	GENERATED_BODY()

		FGameplayTagStack()
	{}

	FGameplayTagStack(FGameplayTag InTag, int32 InStackCount)
		: Tag(InTag)
		, StackCount(InStackCount)
	{
	}

private:
	friend FGameplayTagStackList;

	UPROPERTY()
		FGameplayTag Tag;

	UPROPERTY()
		int32 StackCount = 0;
};

USTRUCT(BlueprintType)
struct FGameplayTagStackList
{
	GENERATED_BODY()

		FGameplayTagStackList()
	{
	}

public:
	// ���ø���Ʈ�� �ױ׸� �߰��մϴ� (����ī��Ʈ�� 1���� ������ �ƹ��͵� ���� �ʽ��ϴ�.)
	// �����ϴ� ����ī��Ʈ�� ������ �����߰�, �ִٸ� 1�����մϴ�.
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// ���ø���Ʈ�� �ױ׸� �����մϴ� (����ī��Ʈ�� 1���� ������ �ƹ��͵� ���� �ʽ��ϴ�.)
	// �����ϴ� ����ī��Ʈ�� StackCount���� ũ�ٸ� StackCount�����մϴ�. StackCount���� ������ �����մϴ�.
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// �±׿� �´� ����ī��Ʈ�� �����ɴϴ�. �±װ� �������� ������ 0�� �����մϴ�.
	int32 GetStackCount(FGameplayTag Tag) const;

	// �±׸� ������ �ִ��� üũ�մϴ�.
	bool ContainsTag(FGameplayTag Tag) const;

private:
	// �����÷��� �ױ� ���� ����Ʈ
	UPROPERTY()
		TArray<FGameplayTagStack> Stacks;
};

/**
 * ������ �κ��丮 ������ �ϳ��� ������ ������ �ֽ��ϴ�.
 */
UCLASS(BlueprintType)
class ACTIONWORLDRPG_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	UFUNCTION(BlueprintCallable, Category = Inventory)
		int32 GetStatTagStackCount(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool HasStatTag(FGameplayTag Tag) const;

	TSubclassOf<UInventoryItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure = false, meta = (DeterminesOutputType = FragmentClass))
		const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	//���ø�?
	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(TSubclassOf<UInventoryItemDefinition> InDef);

	friend struct FInventoryList;

private:
	//�±� ���� źâ ������ ������ �ֽ��ϴ�.
	UPROPERTY()
		FGameplayTagStackList StatTags;

	// �κ��丮 ������ ����
	UPROPERTY()
		TSubclassOf<UInventoryItemDefinition> ItemDef;
};
