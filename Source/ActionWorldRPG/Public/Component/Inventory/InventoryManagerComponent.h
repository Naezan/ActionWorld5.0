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

//�κ��丮 ������ �ϳ��� ������ ��� �ֽ��ϴ�.
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

	//��� ������ ��������
	TArray<UInventoryItemInstance*> GetAllItems() const;

public:
	
	//������ �߰�
	UInventoryItemInstance* AddItem(TSubclassOf<UInventoryItemDefinition> ItemClass, int32 StackCount);
	//??
	void AddItem(UInventoryItemInstance* Instance);
	//������ ����
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
�κ��丮����Ʈ�� �����մϴ�. �κ��丮 �Ŵ����� �÷��̾ �����ϴ� ��� �������� ������ �ֽ��ϴ�.
�����Լ� : �������߰�, �����ۻ���, �������۰�������(�迭) ��
*/
UCLASS()
class UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//�ƹ��͵� ���� �ʽ��ϴ�.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		bool CanAddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	//���������� �߰�(�ϼ��� ��ҵ�� �ϼ��� �����͸� ���� �߰�)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		UInventoryItemInstance* AddItemDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount = 1);

	//������ �ν��Ͻ� �߰�(�ϼ��� ������ �߰�) ������ ItemDef�� ���� ������� �ʽ��ϴ�.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void AddItemInstance(UInventoryItemInstance* ItemInstance);

	//������ �ν��Ͻ� ����
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = Inventory)
		void RemoveItemInstance(UInventoryItemInstance* ItemInstance);

	//��� ������ ��������
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure = false)
		TArray<UInventoryItemInstance*> GetAllItems() const;

	//�Ʒ��� ������� ���� �� ���� �Լ���

	//InventoryList�� Def�� �����ϴ� �ν��Ͻ��� ��ȯ�մϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory, BlueprintPure)
		UInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;

	//Def�� �����ϴ� ��� �κ��丮 �ν��Ͻ��� ����
	int32 GetTotalItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;
	//Def�� �����ϴ� ��� �κ��丮�� �ν��Ͻ� �� Consume��ŭ ����
	bool ConsumeItemsByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 NumToConsume);

private:
	UPROPERTY()
		FInventoryList InventoryList;
};
