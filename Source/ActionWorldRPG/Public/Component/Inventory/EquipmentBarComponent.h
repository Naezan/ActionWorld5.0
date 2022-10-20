// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "EquipmentBarComponent.generated.h"

/**
슬롯과 활성화인덱스, 장착된 아이템, 슬롯개수가 존재합니다.
현재장비셋팅 + UI에 업데이트해줄 정보를 가지고 있습니다.
내부함수 : 다음비어있는슬롯인덱스Get, 아이템을슬롯에 추가, 활성화슬롯을 현재장착한아이템으로 변경
 */

class UInventoryItemInstance;
class UEquipmentInstance;
class UEquipmentManagerComponent;

//UI등에서 현재 활성화할 장비가무엇인지 등을 관리합니다.
//마우스의 키바인딩때문에 컨트롤러에 붙여줍니다.
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONWORLDRPG_API UEquipmentBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UEquipmentBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//순환탐색하여 활성화슬롯을 설정합니다. + 활성화슬롯의 장비를 장착합니다.
	//마우스 휠업
	UFUNCTION(BlueprintCallable)
		void CycleActiveSlotForward();

	//역순환탐색하여 활성화슬롯을 설정합니다. + 활성화슬롯의 장비를 장착합니다.
	//마우스 휠다운
	UFUNCTION(BlueprintCallable)
		void CycleActiveSlotBackward();

	//활성화 슬롯을 재설정합니다. 장비역시 재설정합니다.
	UFUNCTION(BlueprintCallable)
		void SetActiveSlotIndex(int32 NewIndex);

	//슬롯배열을 통째로 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		TArray<UInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	//현재 활성화중인 아이템을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE	UEquipmentInstance* GetCurrentEquippedItem() const { return EquippedItem.Get(); }

	//활성화 슬롯인덱스를가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	//활성화슬롯의 아이템을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		UInventoryItemInstance* GetActiveSlotItem() const;

	//비어있는슬롯의 인덱스를 가져옵니다. 못찾으면 -1을 리턴합니다.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		int32 GetNextFreeItemSlot() const;

	//아이템을 비어있는 슬롯에 추가합니다.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		void AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item);

	//슬롯의 아이템을 삭제합니다.
	UFUNCTION(BlueprintCallable)
		UInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);
	virtual void BeginPlay() override;

private:
	//슬롯의 아이템을 장착해제합니다.
	void UnequipItemInSlot();
	//슬롯의 아이템을 장착합니다.
	void EquipItemInSlot();

	//장비 매니저를 찾습니다.
	UEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	//슬롯의 개수입니다.
	UPROPERTY()
		int32 NumSlots = 3;

private:
	//인벤토리에 있는 아이템의 슬롯들입니다.
	UPROPERTY()
		TArray<TObjectPtr<UInventoryItemInstance>> Slots;

	UPROPERTY()
		int32 ActiveSlotIndex = -1;

	//장착하고 있는 장비입니다.
	UPROPERTY()
		TObjectPtr<UEquipmentInstance> EquippedItem;
};
