// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentSpawnInfo;

//생성된 장비하나의 정보를 가지고 있습니다.
UCLASS(BlueprintType, Blueprintable)
class ACTIONWORLDRPG_API UEquipmentInstance : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual UWorld* GetWorld() const override final;
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category = Equipment)
		UObject* GetInstigator() const { return Instigator; }

	void SetInstigator(UObject* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category = Equipment)
		APawn* GetPawn() const;

	//굳이???
	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
		APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category = Equipment)
		TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	//현재 장비에 필요한 액터를 스폰합니다. 장비매니저에서 호출합니다.
	virtual void SpawnEquipmentActors(const TArray<FEquipmentSpawnInfo>& ActorsToSpawn);
	//현재 장비에 장착된 액터를 모두 삭제합니다.
	virtual void DestroyEquipmentActors();

	//블루프린트 함수를 호출합니다.
	virtual void OnEquipped();
	virtual void OnUnequipped();

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnEquipped"))
		void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = Equipment, meta = (DisplayName = "OnUnequipped"))
		void K2_OnUnequipped();

private:
	UPROPERTY()
		UObject* Instigator;

	UPROPERTY()
		TArray<AActor*> SpawnedActors;
};
