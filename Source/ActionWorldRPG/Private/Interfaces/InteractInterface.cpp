// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/InteractInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

// Add default functionality here for any IInteractInterface functions that are not pure virtual.

bool IInteractInterface::IsAvailableInteraction_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return false;
}

float IInteractInterface::GetInteractionDuration_Implementation(UPrimitiveComponent* InteractionComponent) const
{
	return 0.0f;
}

void IInteractInterface::GetPreInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const
{
	bShouldSync = false;
	Type = EAbilityTaskNetSyncType::OnlyServerWait;
}

void IInteractInterface::GetPostInteractSyncType_Implementation(bool& bShouldSync, EAbilityTaskNetSyncType& Type, UPrimitiveComponent* InteractionComponent) const
{
	bShouldSync = false;
	Type = EAbilityTaskNetSyncType::OnlyServerWait;
}

FSimpleMulticastDelegate* IInteractInterface::GetTargetCancelInteractionDelegate(UPrimitiveComponent* InteractionComponent)
{
	return nullptr;
}

void IInteractInterface::RegisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor)
{
	if (Interacters.Contains(InteractionComponent))
	{
		TArray<AActor*>& InteractingActors = Interacters[InteractionComponent];
		if (!InteractingActors.Contains(InteractingActor))
		{
			InteractingActors.Add(InteractingActor);
		}
	}
	else
	{
		TArray<AActor*> InteractingActors;
		InteractingActors.Add(InteractingActor);
		Interacters.Add(InteractionComponent, InteractingActors);
	}
}

void IInteractInterface::UnregisterInteracter_Implementation(UPrimitiveComponent* InteractionComponent, AActor* InteractingActor)
{
	if (Interacters.Contains(InteractionComponent))
	{
		TArray<AActor*>& InteractingActors = Interacters[InteractionComponent];
		InteractingActors.Remove(InteractingActor);
	}
}

void IInteractInterface::InteractableCancelInteraction_Implementation(UPrimitiveComponent* InteractionComponent)
{
	if (Interacters.Contains(InteractionComponent))
	{
		FGameplayTagContainer InteractAbilityTagContainer;
		InteractAbilityTagContainer.AddTag(FGameplayTag::RequestGameplayTag("Ability.Interaction"));
	
		TArray<AActor*>& InteractingActors = Interacters[InteractionComponent];
		for (AActor* InteractingActor : InteractingActors)
		{
			UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InteractingActor);
		
			if (ASC)
			{
				ASC->CancelAbilities(&InteractAbilityTagContainer);
			}
		}

		InteractingActors.Empty();
	}
}
