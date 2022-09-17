// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WorldInteractionInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWorldInteractionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ACTIONWORLDRPG_API IWorldInteractionInterface
{
	GENERATED_BODY()

public:
	//WorldInteraction
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "WorldInteraction")
		void WorldInteractionProcess();
	virtual void WorldInteractionProcess_Implementation();
};
