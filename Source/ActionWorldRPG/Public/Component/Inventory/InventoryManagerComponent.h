// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/InventoryItem.h"
#include "Interfaces/InventoryInterface.h"
#include "InventoryManagerComponent.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

		FItemData()
		: ItemCount(1)
	{}

	FItemData(int32 InItemCount)
		: ItemCount(InItemCount)
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 ItemCount;

	//�� �񱳿�
	bool operator==(const FItemData& Data) const
	{
		return ItemCount == Data.ItemCount;
	}

	bool operator!=(const FItemData& Data) const
	{
		return !(*this == Data);
	}

	bool IsValid() const
	{
		return ItemCount > 0;
	}

	//�����Ͱ����� �߰���
	void UpdateItemData(const FItemData& Data, int MaxCount)
	{
		if (MaxCount <= 0)
		{
			MaxCount = INT32_MAX;
		}

		//ũ�� ����
		ItemCount = FMath::Clamp(ItemCount + Data.ItemCount, ItemCount, MaxCount);
	}
};

USTRUCT(BlueprintType)
struct FEquipItemType
{
	GENERATED_BODY()

		FEquipItemType()
		: EquipIndex(-1)
	{}

	FEquipItemType(const FPrimaryAssetType& InItemType, int32 InEquipIndex)
		: ItemType(InItemType)
		, EquipIndex(InEquipIndex)
	{}

	//������ ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		FPrimaryAssetType ItemType;

	//�ֹ��� �������� ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 EquipIndex;

	bool operator==(const FEquipItemType& Data) const
	{
		return ItemType == Data.ItemType && EquipIndex == Data.EquipIndex;
	}

	bool operator!=(const FEquipItemType& Data) const
	{
		return !(*this == Data);
	}

	friend inline uint32 GetTypeHash(const FEquipItemType& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.EquipIndex);
		return Hash;
	}

	bool IsValid() const
	{
		return ItemType.IsValid() && EquipIndex >= 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UInventoryItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UInventoryItem*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemChanged, FEquipItemType, EquipItem, UInventoryItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemChangedNative, FEquipItemType, UInventoryItem*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

UCLASS()
class UInventoryManagerComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent() {}

	//�κ��丮�� �ִ� �����۵� ���� ����(Ű : �����ۿ���, �� : ������ ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<UInventoryItem*, FItemData> InventoryData;

	//���� �������� �����۵�(Ű : ��������, �� : �����ۿ���)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FEquipItemType, UInventoryItem*> EquippedItems;

	//Begin �κ��丮 ��������Ʈ
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryItemChanged OnInventoryItemChanged;
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnEquippedItemChanged OnEquippedItemChanged;
	FOnEquippedItemChangedNative OnEquippedItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryLoaded OnInventoryLoaded;
	FOnInventoryLoadedNative OnInventoryLoadedNative;
	//End �κ��丮 ��������Ʈ

	//FOnInventoryItemChanged ���ε� �Լ�
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void InventoryItemChanged(bool bAdded, UInventoryItem* Item);

	//FOnEquippedItemChanged ���ε� �Լ�
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void EquippedItemChanged(FEquipItemType EquipItemType, UInventoryItem* Item);

	//���â�� ����ִٸ� �ڵ����� �����մϴ�. �ƴϸ� �κ��丮�� ���ϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool AddInventoryItem(UInventoryItem* NewItem, int32 ItemCount = 1, bool bAutoEquip = true);

	//�κ��丮�� �ִ� �������� ������ ���ҽ�Ű�ų� ��������ü�� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool RemoveInventoryItem(UInventoryItem* RemoveItem, int32 RemoveItemCount = 1);

	//������ Ÿ�Կ� �´� �κ��丮�� ����������� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetInventoryItemsForType(TArray<UInventoryItem*>& /*out*/ Items, FPrimaryAssetType ItemType);

	//�κ��丮�� �ִ� �������� ������ �����ɴϴ�.
	UFUNCTION(BlueprintPure, Category = Inventory)
		int32 GetInventoryItemCount(UInventoryItem* Item) const;

	//�κ��丮�� �ִ� ������ �����͸������ɴϴ�.(���� �����Ϳ��� ��������)
	UFUNCTION(BlueprintPure, Category = Inventory)
		void GetInventoryItemData(UInventoryItem* Item, FItemData& /*out*/ ItemData) const;

	//������ ����� Ÿ���� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SetEquippedItem(FEquipItemType EquipItemType, UInventoryItem* Item);

	//������ Ÿ������ ��� ã�ƿɴϴ�.
	UFUNCTION(BlueprintPure, Category = Inventory)
		UInventoryItem* GetEquippedItem(FEquipItemType EquipItemType) const;

	//������ Ÿ�Կ� �´� �����ϰ� �ִ� ��� ��� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetAllEquippedItems(TArray<UInventoryItem*>& /*out*/ Items, FPrimaryAssetType ItemType);

	//�ƹ��ǹ̾��� ����ִ� ���Կ� �κ��丮�� �������� ������� ä���ֽ��ϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
	void FillEmptyEquippedItems();

	//Save / Load �ʿ��ϸ�?

	// Implement IInventoryInterface
	virtual const TMap<UInventoryItem*, FItemData>& GetInventoryDataMap() const override
	{
		return InventoryData;
	}
	virtual const TMap<FEquipItemType, UInventoryItem*>& GetEquippedItemMap() const override
	{
		return EquippedItems;
	}
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}
	virtual FOnEquippedItemChangedNative& GetEquippedItemChangedDelegate() override
	{
		return OnEquippedItemChangedNative;
	}
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return OnInventoryLoadedNative;
	}
};
