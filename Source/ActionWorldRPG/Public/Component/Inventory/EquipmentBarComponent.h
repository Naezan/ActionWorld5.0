// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "EquipmentBarComponent.generated.h"

/**
���԰� Ȱ��ȭ�ε���, ������ ������, ���԰����� �����մϴ�.
���������� + UI�� ������Ʈ���� ������ ������ �ֽ��ϴ�.
�����Լ� : ��������ִ½����ε���Get, �����������Կ� �߰�, Ȱ��ȭ������ ���������Ѿ��������� ����
 */

class UInventoryItemInstance;
class UEquipmentInstance;
class UEquipmentManagerComponent;

//UI��� ���� Ȱ��ȭ�� ��񰡹������� ���� �����մϴ�.
//���콺�� Ű���ε������� ��Ʈ�ѷ��� �ٿ��ݴϴ�.
UCLASS(Blueprintable, meta = (BlueprintSpawnableComponent))
class ACTIONWORLDRPG_API UEquipmentBarComponent : public UControllerComponent
{
	GENERATED_BODY()
public:
	UEquipmentBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//��ȯŽ���Ͽ� Ȱ��ȭ������ �����մϴ�. + Ȱ��ȭ������ ��� �����մϴ�.
	//���콺 �پ�
	UFUNCTION(BlueprintCallable)
		void CycleActiveSlotForward();

	//����ȯŽ���Ͽ� Ȱ��ȭ������ �����մϴ�. + Ȱ��ȭ������ ��� �����մϴ�.
	//���콺 �ٴٿ�
	UFUNCTION(BlueprintCallable)
		void CycleActiveSlotBackward();

	//Ȱ��ȭ ������ �缳���մϴ�. ��񿪽� �缳���մϴ�.
	UFUNCTION(BlueprintCallable)
		void SetActiveSlotIndex(int32 NewIndex);

	//���Թ迭�� ��°�� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		TArray<UInventoryItemInstance*> GetSlots() const
	{
		return Slots;
	}

	//���� Ȱ��ȭ���� �������� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE	UEquipmentInstance* GetCurrentEquippedItem() const { return EquippedItem.Get(); }

	//Ȱ��ȭ �����ε����������ɴϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	//Ȱ��ȭ������ �������� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		UInventoryItemInstance* GetActiveSlotItem() const;

	//����ִ½����� �ε����� �����ɴϴ�. ��ã���� -1�� �����մϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure = false)
		int32 GetNextFreeItemSlot() const;

	//�������� ����ִ� ���Կ� �߰��մϴ�.
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		void AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item);

	//������ �������� �����մϴ�.
	UFUNCTION(BlueprintCallable)
		UInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);
	virtual void BeginPlay() override;

private:
	//������ �������� ���������մϴ�.
	void UnequipItemInSlot();
	//������ �������� �����մϴ�.
	void EquipItemInSlot();

	//��� �Ŵ����� ã���ϴ�.
	UEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	//������ �����Դϴ�.
	UPROPERTY()
		int32 NumSlots = 3;

private:
	//�κ��丮�� �ִ� �������� ���Ե��Դϴ�.
	UPROPERTY()
		TArray<TObjectPtr<UInventoryItemInstance>> Slots;

	UPROPERTY()
		int32 ActiveSlotIndex = -1;

	//�����ϰ� �ִ� ����Դϴ�.
	UPROPERTY()
		TObjectPtr<UEquipmentInstance> EquippedItem;
};
