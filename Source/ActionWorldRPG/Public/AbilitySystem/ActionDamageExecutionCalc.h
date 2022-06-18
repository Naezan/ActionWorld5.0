// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ActionDamageExecutionCalc.generated.h"

//데미지를 공격과 방어력에따라서 계산해서 실행해주는 클래스

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UActionDamageExecutionCalc : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	// Constructor and overrides
	UActionDamageExecutionCalc();
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, OUT FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	float HeadShotMultiplier;
};
