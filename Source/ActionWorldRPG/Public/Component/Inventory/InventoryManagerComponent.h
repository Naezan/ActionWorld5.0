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

	//값 비교용
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

	//데이터같을시 추가용
	void UpdateItemData(const FItemData& Data, int MaxCount)
	{
		if (MaxCount <= 0)
		{
			MaxCount = INT32_MAX;
		}

		//크기 조정
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

	//데이터 에셋정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		FPrimaryAssetType ItemType;

	//주무기 보조무기 근접무기
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

	//인벤토리에 있는 아이템들 무기 포함(키 : 아이템에셋, 값 : 아이템 개수)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<UInventoryItem*, FItemData> InventoryData;

	//현재 장착중인 아이템들(키 : 무기정보, 값 : 아이템에셋)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory)
		TMap<FEquipItemContainer, UInventoryItem*> EquippedItems;

	//Begin 인벤토리 델리게이트
	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryItemChanged OnInventoryItemChanged;
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnEquippedItemChanged OnEquippedItemChanged;
	FOnEquippedItemChangedNative OnEquippedItemChangedNative;

	UPROPERTY(BlueprintAssignable, Category = Inventory)
		FOnInventoryLoaded OnInventoryLoaded;
	FOnInventoryLoadedNative OnInventoryLoadedNative;
	//End 인벤토리 델리게이트

	//FOnInventoryItemChanged 바인드 함수
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void InventoryItemChanged(bool bAdded, UInventoryItem* Item);

	//FOnEquippedItemChanged 바인드 함수
	UFUNCTION(BlueprintImplementableEvent, Category = Inventory)
		void EquippedItemChanged(FEquipItemContainer EquipItemType, UInventoryItem* Item);

	//장비창이 비어있다면 자동으로 장착합니다. 아니면 인벤토리로 들어갑니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool AddInventoryItem(UInventoryItem* NewItem, int32 ItemCount = 1, bool bAutoEquip = true);

	//인벤토리에 있는 아이템의 수량을 감소시키거나 아이템자체를 제거합니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool RemoveInventoryItem(UInventoryItem* RemoveItem, int32 RemoveItemCount = 1);

	//데이터 타입에 맞는 인벤토리의 아이템을모두 가져옵니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetInventoryItemsForType(TArray<UInventoryItem*>& /*out*/ Items, FPrimaryAssetType ItemType);

	//인벤토리에 있는 아이템의 갯수를 가져옵니다.
	UFUNCTION(BlueprintPure, Category = Inventory)
		int32 GetInventoryItemCount(UInventoryItem* Item) const;

	//인벤토리에 있는 아이템 데이터를가져옵니다.(현재 데이터에는 갯수뿐임)
	UFUNCTION(BlueprintPure, Category = Inventory)
		bool GetInventoryItemData(UInventoryItem* Item, FItemData& /*out*/ ItemData) const;

	//장착된 장비의 타입을 셋팅합니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		bool SetEquippedItem(FEquipItemContainer EquipItemType, UInventoryItem* NewItem);

	//아이템 타입으로 장비를 찾아옵니다.
	UFUNCTION(BlueprintPure, Category = Inventory)
		UInventoryItem* GetEquippedItem(FEquipItemContainer EquipItemType) const;

	//데이터 타입에 맞는 장착하고 있는 모든 장비를 가져옵니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void GetAllEquippedItems(TArray<UInventoryItem*>& /*out*/ Items, FPrimaryAssetType ItemType);

	//아무의미없이 비어있는 슬롯에 인벤토리의 아이템을 순서대로 채워넣습니다.
	UFUNCTION(BlueprintCallable, Category = Inventory)
		void FillEmptyEquippedItems();

	//Save / Load 필요하면?

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
	//특정 아이템을 비어있는 장비창에 넣습니다.
	bool FillEmptyEquippedItem(UInventoryItem* NewItem);

	//Inventory Delegate Bind Function
	void NotifyInventoryItemChanged(bool bAdded, UInventoryItem* Item);
	void NotifyEquippedItemChanged(FEquipItemContainer ItemType, UInventoryItem* Item);
	void NotifyInventoryChanged();
};
