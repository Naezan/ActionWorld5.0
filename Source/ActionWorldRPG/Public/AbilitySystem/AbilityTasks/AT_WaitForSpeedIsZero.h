// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AT_WaitForSpeedIsZero.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWaitSpeedDelegate);

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UAT_WaitForSpeedIsZero : public UAbilityTask
{
	GENERATED_UCLASS_BODY()
	
	UPROPERTY(BlueprintAssignable)
	FWaitSpeedDelegate	OnNearToZero;

	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "WaitForSpeedIsZero", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
		static UAT_WaitForSpeedIsZero* CreateWaitForZeroSpeed(UGameplayAbility* OwningAbility, FName TaskInstanceName);

	virtual void Activate() override;

	UFUNCTION()
	void OnSpeedIsNearToZero();

protected:

	virtual void OnDestroy(bool AbilityEnded) override;
};
