// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/ActionDeathGameplayAbility.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "Character/ActionCharacterBase.h"

void UActionDeathGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (ActorInfo == nullptr)
	{
		return;
	}

	UActionAbilitySystemComponent* ASC = Cast<UActionAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
	//WithTags를 가지고있고 WithoutTags를 안가지고 있다면 데스어빌리티를 캔슬합니다.
	ASC->CancelAbilities(nullptr, nullptr, this);

	//이 어빌리티는 절대 캔슬될 수 없습니다.
	SetCanBeCanceled(false);

	StartDeath();

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}

void UActionDeathGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (ActorInfo == nullptr)
	{
		return;
	}

	FinishDeath();

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UActionDeathGameplayAbility::StartDeath()
{
	if (AActionCharacterBase* AvatarActor = Cast<AActionCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		AvatarActor->StartDeath();
	}
}

void UActionDeathGameplayAbility::FinishDeath()
{
	if (AActionCharacterBase* AvatarActor = Cast<AActionCharacterBase>(GetAvatarActorFromActorInfo()))
	{
		AvatarActor->FinishDeath();
	}
}
