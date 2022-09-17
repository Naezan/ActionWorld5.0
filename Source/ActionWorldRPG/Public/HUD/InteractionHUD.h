// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionHUD.generated.h"

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API UInteractionHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void ShowInteractionPrompt(float InteractionDuration);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void HideInteractionPrompt();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StartInteractionTimer(float InteractionDuration);

	// Interaction interrupted, cancel and hide HUD interact timer
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StopInteractionTimer();
};
