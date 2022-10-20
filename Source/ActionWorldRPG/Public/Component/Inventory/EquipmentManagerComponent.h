// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

/**
장비리스트가 존재합니다. 장비 매니저는 장착중인3가지의 슬롯 아이템을 가지고 있습니다만 현재 장착하고있는 아이템1개만을 가지게 됩니다.
리스트이지만 가지고 있는 장비는 1개입니다.
내부함수 : 장비장착, 장비해제 등
 */

struct FEquipmentList;
class UEquipmentDefinition;
class UEquipmentInstance;
class UActionAbilitySystemComponent;

//배열의 요소를 가지고 있는 구조체로 바꿉니다.
USTRUCT(BlueprintType)
struct FEquipmentItem
{
	GENERATED_BODY()

		FEquipmentItem()
	{}

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	// 장착한 장비
	UPROPERTY()
		TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
		UEquipmentInstance* Instance = nullptr;

	// 무기가 사용할 어빌리티셋이지만 내가 만든것에는 무기에 능력이 포함되어 있다.
	//UPROPERTY(NotReplicated)
	//	FAbilitySet_GrantedHandles GrantedHandles;
};

//배열을 가지고있는 구조체로 바꿉니다.
USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

		FEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FEquipmentList(UActorComponent* InOwnerComponent)
		: OwnerComponent(InOwnerComponent)
	{
	}


	//장비 추가함수 제거함수 EquipItem, UnequipItem 호출시에 호출됩니다.
	UEquipmentInstance* AddEquipmentItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void RemoveEquipmentItem(UEquipmentInstance* Instance);

private:
	UActionAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UEquipmentManagerComponent;

private:
	// 장비들
	UPROPERTY()
		TArray<FEquipmentItem> EquipmentItems;

	UPROPERTY()
		UActorComponent* OwnerComponent;
};

//가지고 있는 장비들의 전체적인 시스템을 담당합니다.
//캐릭터의 몸에 붙이기위해서 폰컴포넌트에 할당해줍니다.
UCLASS()
class ACTIONWORLDRPG_API UEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()
	
public:
	UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		UEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
		void UnequipItem(UEquipmentInstance* ItemInstance);

	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;

	//가지고 있는 장비에서 첫번째로 생성된 인스턴스를 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceType);

	//가지고 있는 모든 장비의 인스턴스들을 가져옵니다.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		TArray<UEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
		FEquipmentList EquipmentList;
};
