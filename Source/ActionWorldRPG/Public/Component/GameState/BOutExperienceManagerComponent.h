// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "BOutExperienceManagerComponent.generated.h"

/**
 * tipically used to Activate GameFeaturePlugins
 */
UCLASS()
class ACTIONWORLDRPG_API UBOutExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()

public:
	//Begin ActorComponent
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//End ActorComponent

	UFUNCTION(BlueprintCallable)
		void LoadAndActivateGameFeaturePlugin(FString GameFeaturePluginName, bool bEnable);
};
