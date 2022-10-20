// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryItemDefinition.generated.h"

class UInventoryItemInstance;

//레벨 추가 방지, 프로퍼티창에서 인스턴스화해서 사용합니다.
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const {}
};

//장비나 UI에 쓰일 아이템의 거의 모든 정보를 가지고 있습니다. EquipmentDefinition를 가질 수 있습니다.
UCLASS(Blueprintable, Const, Abstract)
class ACTIONWORLDRPG_API UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//무기정보(EquipmentDefinition) 및 UI정보 등 아이템으로서 존재하는 거의 모든 정보를 가지고 있습니다.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Display, Instanced)
		TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	//알맞는 Fragment정보를 찾습니다.
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
};

UCLASS()
class UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		//FragmentClass의 리턴타입을 가집니다.
		UFUNCTION(BlueprintCallable, meta = (DeterminesOutputType = FragmentClass))
		static const UInventoryItemFragment* FindItemDefinitionFragment(TSubclassOf<UInventoryItemDefinition> ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass);
};

