// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "InventoryItemInstance.generated.h"

class UInventoryItemDefinition;

//탄창을 저장할 때 사용합니다.
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
	// 스택리스트에 테그를 추가합니다 (스택카운트가 1보다 작으면 아무것도 하지 않습니다.)
	// 존재하는 스택카운트가 없으면 새로추가, 있다면 1증가합니다.
	void AddStack(FGameplayTag Tag, int32 StackCount);

	// 스택리스트에 테그를 제거합니다 (스택카운트가 1보다 작으면 아무것도 하지 않습니다.)
	// 존재하는 스택카운트가 StackCount보다 크다면 StackCount감소합니다. StackCount보다 작으면 제거합니다.
	void RemoveStack(FGameplayTag Tag, int32 StackCount);

	// 태그에 맞는 스택카운트를 가져옵니다. 태그가 존재하지 않으면 0을 리턴합니다.
	int32 GetStackCount(FGameplayTag Tag) const;

	// 태그를 가지고 있는지 체크합니다.
	bool ContainsTag(FGameplayTag Tag) const;

private:
	// 게임플레이 테그 스택 리스트
	UPROPERTY()
		TArray<FGameplayTagStack> Stacks;
};

/**
 * 생성할 인벤토리 아이템 하나의 정보를 가지고 있습니다.
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

	//템플릿?
	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}

private:
	void SetItemDef(TSubclassOf<UInventoryItemDefinition> InDef);

	friend struct FInventoryList;

private:
	//태그 정보 탄창 정보를 가지고 있습니다.
	UPROPERTY()
		FGameplayTagStackList StatTags;

	// 인벤토리 아이템 정보
	UPROPERTY()
		TSubclassOf<UInventoryItemDefinition> ItemDef;
};
