// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
//¾îºô¸®Æ¼
#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystem/ActionAbilityTypes.h"
#include "ActionTargetType.generated.h"

class AActionCharacterBase;
class AActor;
struct FGameplayEventData;

/**
 * 
 */
UCLASS(Blueprintable, meta = (ShowWorldContextPin))
class ACTIONWORLDRPG_API UActionTargetType : public UObject
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UActionTargetType() {}

	/** Called to determine targets to apply gameplay effects to */
	UFUNCTION(BlueprintNativeEvent)
		void GetTargets(AActionCharacterBase* TargetingCharacter, AActor* TargetingActor, 
		FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, 
		TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;
	virtual void GetTargets_Implementation(AActionCharacterBase* TargetingCharacter, AActor* TargetingActor, FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const;

};

/** Trivial target type that uses the owner */
UCLASS(NotBlueprintable)
class ACTIONWORLDRPG_API UActionTargetType_UseOwner : public UActionTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UActionTargetType_UseOwner() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AActionCharacterBase* TargetingCharacter, AActor* TargetingActor,
	FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, 
	TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};

/** Trivial target type that pulls the target out of the event data */
UCLASS(NotBlueprintable)
class ACTIONWORLDRPG_API UActionTargetType_UseEventData : public UActionTargetType
{
	GENERATED_BODY()

public:
	// Constructor and overrides
	UActionTargetType_UseEventData() {}

	/** Uses the passed in event data */
	virtual void GetTargets_Implementation(AActionCharacterBase* TargetingCharacter, AActor* TargetingActor,
	FGameplayEventData EventData, TArray<FGameplayAbilityTargetDataHandle>& OutTargetData, 
	TArray<FHitResult>& OutHitResults, TArray<AActor*>& OutActors) const override;
};