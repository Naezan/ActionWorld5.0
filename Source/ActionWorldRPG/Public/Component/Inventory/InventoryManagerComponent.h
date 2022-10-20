// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryItem.h"
#include "Interfaces/InventoryInterface.h"
#include "InventoryManagerComponent.generated.h"

class UInventoryItemDefinition;
class UInventoryItemInstance;
class UInventoryManagerComponent;
struct FInventoryList;

//인벤토리 아이템 하나의 정보를 담고 있습니다.
USTRUCT(BlueprintType)
struct FInventoryItemInfo
{
	GENERATED_BODY()

		FInventoryItemInfo()
	{}

private:
	friend FInventoryList;
	friend UInventoryManagerComponent;

	UPROPERTY()
		UInventoryItemInstance* Instance = nullptr;

	UPROPERTY()
		int32 StackCount = 0;

	UPROPERTY(NotReplicated)
		int32 LastObservedCount = INDEX_NONE;
};

USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

		FInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FInventoryList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}

	//모든 아이템 가져오기
	TArray<UInventoryItemInstance*> GetAllItems() const;

public:
	
	//아이템 추가
	UInventoryItemInstance* AddItem(TSubclassOf<UInventoryItemDefinition> ItemClass, int32 StackCount);
	//??
	void AddItem(UInventoryItemInstance* Instance);
	//아이템 제거
	void RemoveItem(UInventoryItemInstance* Instance);

private:
	friend UInventoryManagerComponent;

private:
	// Replicated list of items
	UPROPERTY()
		TArray<FInventoryItemInfo> Items;

	UPROPERTY()
		UActorComponent* OwnerComponent;
};

/*
인벤토리리스트가 존재합니다. 인벤토리 매니저는 플레이어가 소유하는 모든 아이템을 가지고 있습니다.
내부함수 : 아이템추가, 아이템삭제, 모든아이템가져오기(배열) 등
*/
UCLASS()
class UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//아무것도 하지 않습니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		bool CanAddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	//아이템정보 추가(완성된 요소들로 완성된 데이터를 만들어서 추가)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		UInventoryItemInstance* AddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	//아이템 인스턴스 추가(완성된 데이터 추가) 하지만 ItemDef가 없다 사용하지 않습니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void AddItemInstance(UInventoryItemInstance* ItemInstance);

	//아이템 인스턴스 제거
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void RemoveItemInstance(UInventoryItemInstance* ItemInstance);

	//모든 아이템 가져오기
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
		TArray<UInventoryItemInstance*> GetAllItems() const;

	//아래로 사용하지 않을 것 같은 함수들

	//InventoryList에 Def로 존재하는 인스턴스를 반환합니다.
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
		UInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;

	//Def로 존재하는 모든 인벤토리 인스턴스의 개수
	int32 GetTotalItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;
	//Def로 존재하는 모든 인벤토리의 인스턴스 중 Consume만큼 제거
	bool ConsumeItemsByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 NumToConsume);

private:
	UPROPERTY()
		FInventoryList InventoryList;
};
