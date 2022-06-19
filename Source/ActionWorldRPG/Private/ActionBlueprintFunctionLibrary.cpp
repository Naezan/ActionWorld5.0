// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionBlueprintFunctionLibrary.h"
#include "..\Public\ActionBlueprintFunctionLibrary.h"

FString UActionBlueprintFunctionLibrary::GetPlayerEditorWindowRole(UWorld* World)
{
	FString Prefix;
	if (World)
	{
		if (World->WorldType == EWorldType::PIE)
		{
			switch (World->GetNetMode())
			{
			case NM_Client:
				Prefix = FString::Printf(TEXT("Client %d "), GPlayInEditorID - 1);
				break;
			case NM_DedicatedServer:
			case NM_ListenServer:
				Prefix = FString::Printf(TEXT("Server "));
				break;
			case NM_Standalone:
				break;
			}
		}
	}

	return Prefix;
}

UActionGameplayAbility* UActionBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromHandle(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UActionGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

UActionGameplayAbility* UActionBlueprintFunctionLibrary::GetPrimaryAbilityInstanceFromClass(UAbilitySystemComponent* AbilitySystemComponent, TSubclassOf<UGameplayAbility> InAbilityClass)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(InAbilityClass);
		if (AbilitySpec)
		{
			return Cast<UActionGameplayAbility>(AbilitySpec->GetPrimaryInstance());
		}
	}

	return nullptr;
}

bool UActionBlueprintFunctionLibrary::IsPrimaryAbilityInstanceActive(UAbilitySystemComponent* AbilitySystemComponent, FGameplayAbilitySpecHandle Handle)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromHandle(Handle);
		if (AbilitySpec)
		{
			return Cast<UActionGameplayAbility>(AbilitySpec->GetPrimaryInstance())->IsActive();
		}
	}

	return false;
}

bool UActionBlueprintFunctionLibrary::IsAbilitySpecHandleValid(FGameplayAbilitySpecHandle Handle)
{
	return Handle.IsValid();
}

void UActionBlueprintFunctionLibrary::ClearTargetData(UPARAM(ref)FGameplayAbilityTargetDataHandle& TargetData)
{
	TargetData.Clear();
}
