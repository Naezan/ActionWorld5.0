// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemSpawnableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UItemSpawnableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONWORLDRPG_API IItemSpawnableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void SpawnItems(UClass* ItemClass, FTransform const& Transform, int32 ItemCount = 1);
};
