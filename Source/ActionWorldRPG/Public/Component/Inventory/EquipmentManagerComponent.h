// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentManagerComponent.generated.h"

/**
��񸮽�Ʈ�� �����մϴ�. ��� �Ŵ����� ��������3������ ���� �������� ������ �ֽ��ϴٸ� ���� �����ϰ��ִ� ������1������ ������ �˴ϴ�.
����Ʈ������ ������ �ִ� ���� 1���Դϴ�.
�����Լ� : �������, ������� ��
 */

struct FEquipmentList;
class UEquipmentDefinition;
class UEquipmentInstance;
class UActionAbilitySystemComponent;

//�迭�� ��Ҹ� ������ �ִ� ����ü�� �ٲߴϴ�.
USTRUCT(BlueprintType)
struct FEquipmentItem
{
	GENERATED_BODY()

		FEquipmentItem()
	{}

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	// ������ ���
	UPROPERTY()
		TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
		UEquipmentInstance* Instance = nullptr;

	// ���Ⱑ ����� �����Ƽ�������� ���� ����Ϳ��� ���⿡ �ɷ��� ���ԵǾ� �ִ�.
	//UPROPERTY(NotReplicated)
	//	FAbilitySet_GrantedHandles GrantedHandles;
};

//�迭�� �������ִ� ����ü�� �ٲߴϴ�.
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


	//��� �߰��Լ� �����Լ� EquipItem, UnequipItem ȣ��ÿ� ȣ��˴ϴ�.
	UEquipmentInstance* AddEquipmentItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void RemoveEquipmentItem(UEquipmentInstance* Instance);

private:
	UActionAbilitySystemComponent* GetAbilitySystemComponent() const;

	friend UEquipmentManagerComponent;

private:
	// ����
	UPROPERTY()
		TArray<FEquipmentItem> EquipmentItems;

	UPROPERTY()
		UActorComponent* OwnerComponent;
};

//������ �ִ� ������ ��ü���� �ý����� ����մϴ�.
//ĳ������ ���� ���̱����ؼ� ��������Ʈ�� �Ҵ����ݴϴ�.
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

	//������ �ִ� ��񿡼� ù��°�� ������ �ν��Ͻ��� �����ɴϴ�.
	UFUNCTION(BlueprintCallable, BlueprintPure)
		UEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceType);

	//������ �ִ� ��� ����� �ν��Ͻ����� �����ɴϴ�.
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
