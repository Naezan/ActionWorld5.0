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
	//WithTags�� �������ְ� WithoutTags�� �Ȱ����� �ִٸ� ���������Ƽ�� ĵ���մϴ�.
	ASC->CancelAbilities(nullptr, nullptr, this);

	//�� �����Ƽ�� ���� ĵ���� �� �����ϴ�.
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
