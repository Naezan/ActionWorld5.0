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
struct FEquipItemContainer
{
	GENERATED_BODY()

		FEquipItemContainer()
		: EquipItemIndex(-1)
	{}

	FEquipItemContainer(const FPrimaryAssetType& InItemType, int32 InEquipItemIndex)
		: ItemType(InItemType)
		, EquipItemIndex(InEquipItemIndex)
	{}

	//������ ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		FPrimaryAssetType ItemType;

	//�ֹ��� �������� ��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		int32 EquipItemIndex;

	bool operator==(const FEquipItemContainer& Data) const
	{
		return ItemType == Data.ItemType && EquipItemIndex == Data.EquipItemIndex;
	}

	bool operator!=(const FEquipItemContainer& Data) const
	{
		return !(*this == Data);
	}

	friend inline uint32 GetTypeHash(const FEquipItemContainer& Key)
	{
		uint32 Hash = 0;

		Hash = HashCombine(Hash, GetTypeHash(Key.ItemType));
		Hash = HashCombine(Hash, (uint32)Key.EquipItemIndex);
		return Hash;
	}

	bool IsValid() const
	{
		return ItemType.IsValid() && EquipItemIndex >= 0;
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UInventoryItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UInventoryItem*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemChanged, FEquipItemContainer, EquipItem, UInventoryItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemChangedNative, FEquipItemContainer, UInventoryItem*);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);

UCLASS()
class UInventoryManagerComponent : public UActorComponent, public IInventoryInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent() {}

	virtual void BeginPlay() override;

	//�κ��丮�� �ִ� �����۵� ���� ����(Ű : �����ۿ���, �� : ������ ����)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<UInventoryItem*, FItemData> InventoryData;

	//���� �������� �����۵�(Ű : ��������, �� : �����ۿ���)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FEquipItemContainer, UInventoryItem*> EquippedItems;

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
		void EquippedItemChanged(FEquipItemContainer EquipItemType, UInventoryItem* Item);

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
		bool GetInventoryItemData(UInventoryItem* Item, FItemData& /*out*/ ItemData) const;

	//������ ����� Ÿ���� �����մϴ�.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SetEquippedItem(FEquipItemContainer EquipItemType, UInventoryItem* NewItem);

	//������ Ÿ������ ��� ã�ƿɴϴ�.
	UFUNCTION(BlueprintPure, Category = Inventory)
		UInventoryItem* GetEquippedItem(FEquipItemContainer EquipItemType) const;

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
	virtual const TMap<FEquipItemContainer, UInventoryItem*>& GetEquippedItemMap() const override
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

protected:
	//Ư�� �������� ����ִ� ���â�� �ֽ��ϴ�.
	bool FillEmptyEquippedItem(UInventoryItem* NewItem);

	//Inventory Delegate Bind Function
	void NotifyInventoryItemChanged(bool bAdded, UInventoryItem* Item);
	void NotifyEquippedItemChanged(FEquipItemContainer ItemType, UInventoryItem* Item);
	void NotifyInventoryChanged();
};
