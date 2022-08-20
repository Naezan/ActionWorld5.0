// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ModularPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API AModularPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//Begin AActor
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//End AActor

	//Begin APlayerController
	virtual void ReceivedPlayer() override;
	//End APlayerController
};
