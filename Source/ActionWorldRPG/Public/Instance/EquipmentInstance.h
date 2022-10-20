// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentInstance.generated.h"

struct FEquipmentSpawnInfo;

//������ ����ϳ��� ������ ������ �ֽ��ϴ�.
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

	//����???
	UFUNCTION(BlueprintPure, Category = Equipment, meta = (DeterminesOutputType = PawnType))
		APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	UFUNCTION(BlueprintPure, Category = Equipment)
		TArray<AActor*> GetSpawnedActors() const { return SpawnedActors; }

	//���� ��� �ʿ��� ���͸� �����մϴ�. ���Ŵ������� ȣ���մϴ�.
	virtual void SpawnEquipmentActors(const TArray<FEquipmentSpawnInfo>& ActorsToSpawn);
	//���� ��� ������ ���͸� ��� �����մϴ�.
	virtual void DestroyEquipmentActors();

	//�������Ʈ �Լ��� ȣ���մϴ�.
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
