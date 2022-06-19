// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ActionGameplayAbility.h"

#include "ActionBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UActionBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Returns the player's editor window role - Server (listen host) or Client #
	UFUNCTION(BlueprintCallable)
		static FString GetPlayerEditorWindowRole(UWorld* World);


	/**
	* GameplayAbility
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability")
		static UActionGameplayAbility* GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Ability")
		static UActionGameplayAbility* GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass);

	UFUNCTION(BlueprintCallable, Category = "Ability")
		static bool IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle);


	/**
	* FGameplayAbilitySpecHandle
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ability")
		static bool IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle);

	/**
	* FGameplayAbilityTargetDataHandle
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|TargetData")
		static void ClearTargetData(UPARAM(ref) FGameplayAbilityTargetDataHandle& TargetData);
};
