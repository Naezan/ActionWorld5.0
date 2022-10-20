// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EquipmentDefinition.generated.h"

class UEquipmentInstance;

//스폰에 필요한 정보를담고 있습니다.
USTRUCT()
struct FEquipmentSpawnInfo
{
	GENERATED_BODY()

		FEquipmentSpawnInfo()
	{}

	UPROPERTY(EditAnywhere, Category = Equipment)
		TSubclassOf<AActor> ActorToSpawn;

	UPROPERTY(EditAnywhere, Category = Equipment)
		FName AttachSocket;

	UPROPERTY(EditAnywhere, Category = Equipment)
		FTransform AttachTransform;
};

//장비를 장착할때 필요한 정보들을 담고 있습니다.
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class ACTIONWORLDRPG_API UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// 스폰할 무기
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TSubclassOf<UEquipmentInstance> InstanceType;

	// 무기스폰 관련 정보
	UPROPERTY(EditDefaultsOnly, Category = Equipment)
		TArray<FEquipmentSpawnInfo> ActorsToSpawn;
};
