// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ActionGameplayAbility.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/ActionTargetType.h"

//캐릭터
#include "Character/ActionCharacterBase.h"
#include "Character/Player/PlayerBase.h"

#include "ActionBlueprintFunctionLibrary.h"

UActionGameplayAbility::UActionGameplayAbility()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bActivateAbilityOnGranted = false;
	bActivateOnInput = true;
	bSourceObjectMustEqualCurrentWeaponToActivate = false;
	bCannotActivateWhileInteracting = true;

	// UGSAbilitySystemGlobals hasn't initialized tags yet to set ActivationBlockedTags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.KnockedDown"));

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag("Ability.BlocksInteraction"));

	InteractingTag = FGameplayTag::RequestGameplayTag("State.Interacting");
	InteractingRemovalTag = FGameplayTag::RequestGameplayTag("State.InteractingRemoval");
}

void UActionGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (bActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

FGameplayAbilityTargetDataHandle UActionGameplayAbility::MakeGameplayAbilityTargetDataHandleFromActorArray(const TArray<AActor*> TargetActors)
{
	if (TargetActors.Num() > 0)
	{
		FGameplayAbilityTargetData_ActorArray* NewData = new FGameplayAbilityTargetData_ActorArray();
		NewData->TargetActorArray.Append(TargetActors);
		return FGameplayAbilityTargetDataHandle(NewData);
	}

	return FGameplayAbilityTargetDataHandle();
}

FGameplayAbilityTargetDataHandle UActionGameplayAbility::MakeGameplayAbilityTargetDataHandleFromHitResults(const TArray<FHitResult> HitResults)
{
	FGameplayAbilityTargetDataHandle TargetData;

	for (const FHitResult& HitResult : HitResults)
	{
		FGameplayAbilityTargetData_SingleTargetHit* NewData = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
		TargetData.Add(NewData);
	}

	return TargetData;
}

UObject* UActionGameplayAbility::K2_GetSourceObject(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return GetSourceObject(Handle, &ActorInfo);
}

bool UActionGameplayAbility::BatchRPCTryActivateAbility(FGameplayAbilitySpecHandle InAbilityHandle, bool EndAbilityImmediately)
{
	UActionAbilitySystemComponent* GSASC = Cast<UActionAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo());
	if (GSASC)
	{
		return GSASC->BatchRPCTryActivateAbility(InAbilityHandle, EndAbilityImmediately);
	}

	return false;
}

void UActionGameplayAbility::ExternalEndAbility()
{
	check(CurrentActorInfo);

	bool bReplicateEndAbility = true;
	bool bWasCancelled = false;
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, bReplicateEndAbility, bWasCancelled);

}

FString UActionGameplayAbility::GetCurrentPredictionKeyStatus()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	return ASC->ScopedPredictionKey.ToString() + " is valid for more prediction: " + (ASC->ScopedPredictionKey.IsValidForMorePrediction() ? TEXT("true") : TEXT("false"));

}

bool UActionGameplayAbility::IsPredictionKeyValidForMorePrediction() const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	return ASC->ScopedPredictionKey.IsValidForMorePrediction();
}

bool UActionGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (bCannotActivateWhileInteracting)
	{
		UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
		if (ASC->GetTagCount(InteractingTag) > ASC->GetTagCount(InteractingRemovalTag))
		{
			return false;
		}
	}

	if (bSourceObjectMustEqualCurrentWeaponToActivate)
	{
		APlayerBase* Hero = Cast<APlayerBase>(ActorInfo->AvatarActor);
		if (Hero && Hero->GetCurrentWeapon() && (UObject*)Hero->GetCurrentWeapon() == GetSourceObject(Handle, ActorInfo))
		{
			return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
		}
		else
		{
			return false;
		}
	}

	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

}

bool UActionGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) && CheckCost(Handle, *ActorInfo);

}

bool UActionGameplayAbility::CheckCost_Implementation(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo& ActorInfo) const
{
	return true;
}

void UActionGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	ApplyCost(Handle, *ActorInfo, ActivationInfo);
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);

}

void UActionGameplayAbility::SendTargetDataToServer(const FGameplayAbilityTargetDataHandle& TargetData)
{
	if (IsPredictingClient())
	{
		UAbilitySystemComponent* ASC = CurrentActorInfo->AbilitySystemComponent.Get();
		check(ASC);

		FScopedPredictionWindow	ScopedPrediction(ASC, IsPredictingClient());

		FGameplayTag ApplicationTag; // Fixme: where would this be useful?
		CurrentActorInfo->AbilitySystemComponent->CallServerSetReplicatedTargetData(CurrentSpecHandle,
			CurrentActivationInfo.GetActivationPredictionKey(), TargetData, ApplicationTag, ASC->ScopedPredictionKey);
	}
}

bool UActionGameplayAbility::IsInputPressed() const
{
	FGameplayAbilitySpec* Spec = GetCurrentAbilitySpec();
	return Spec && Spec->InputPressed;
}

UAnimMontage* UActionGameplayAbility::GetCurrentMontageForMesh(USkeletalMeshComponent* InMesh)
{
	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		return AbilityMeshMontage.Montage;
	}

	return nullptr;
}

void UActionGameplayAbility::SetCurrentMontageForMesh(USkeletalMeshComponent* InMesh, UAnimMontage* InCurrentMontage)
{
	ensure(IsInstantiated());

	FAbilityMeshMontage AbilityMeshMontage;
	if (FindAbillityMeshMontage(InMesh, AbilityMeshMontage))
	{
		AbilityMeshMontage.Montage = InCurrentMontage;
	}
	else
	{
		CurrentAbilityMeshMontages.Add(FAbilityMeshMontage(InMesh, InCurrentMontage));
	}
}

bool UActionGameplayAbility::FindAbillityMeshMontage(USkeletalMeshComponent* InMesh, FAbilityMeshMontage& InAbilityMontage)
{
	for (FAbilityMeshMontage& MeshMontage : CurrentAbilityMeshMontages)
	{
		if (MeshMontage.Mesh == InMesh)
		{
			InAbilityMontage = MeshMontage;
			return true;
		}
	}

	return false;
}

void UActionGameplayAbility::MontageJumpToSectionForMesh(USkeletalMeshComponent* InMesh, FName SectionName)
{
	check(CurrentActorInfo);

	UActionAbilitySystemComponent* const AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageJumpToSectionForMesh(InMesh, SectionName);
	}
}

void UActionGameplayAbility::MontageSetNextSectionNameForMesh(USkeletalMeshComponent* InMesh, FName FromSectionName, FName ToSectionName)
{
	check(CurrentActorInfo);

	UActionAbilitySystemComponent* const AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo_Checked());
	if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
	{
		AbilitySystemComponent->CurrentMontageSetNextSectionNameForMesh(InMesh, FromSectionName, ToSectionName);
	}
}

void UActionGameplayAbility::MontageStopForMesh(USkeletalMeshComponent* InMesh, float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UActionAbilitySystemComponent* const AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		// We should only stop the current montage if we are the animating ability
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->CurrentMontageStopForMesh(InMesh, OverrideBlendOutTime);
		}
	}
}

void UActionGameplayAbility::MontageStopForAllMeshes(float OverrideBlendOutTime)
{
	check(CurrentActorInfo);

	UActionAbilitySystemComponent* const AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent.Get());
	if (AbilitySystemComponent != nullptr)
	{
		if (AbilitySystemComponent->IsAnimatingAbilityForAnyMesh(this))
		{
			AbilitySystemComponent->StopAllCurrentMontages(OverrideBlendOutTime);
		}
	}
}
