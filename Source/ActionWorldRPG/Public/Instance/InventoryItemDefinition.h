// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryItemDefinition.generated.h"

class UInventoryItemInstance;

//���� �߰� ����, ������Ƽâ���� �ν��Ͻ�ȭ�ؼ� ����մϴ�.
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const {}
};

//��� UI�� ���� �������� ���� ��� ������ ������ �ֽ��ϴ�. EquipmentDefinition�� ���� �� �ֽ��ϴ�.
UCLASS(Blueprintable, Const, Abstract)
class ACTIONWORLDRPG_API UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//��������(EquipmentDefinition) �� UI���� �� ���������μ� �����ϴ� ���� ��� ������ ������ �ֽ��ϴ�.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
		TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	//�˸´� Fragment������ ã���ϴ�.
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
};

UCLASS()
class UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		//FragmentClass�� ����Ÿ���� �����ϴ�.
		UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
		static const UInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UInventoryItemDefinition> ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass);
};

