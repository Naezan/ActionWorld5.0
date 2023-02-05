// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AT_WaitForSpeedIsZero.h"
#include "Character/Player/PlayerBase.h"

UAT_WaitForSpeedIsZero::UAT_WaitForSpeedIsZero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAT_WaitForSpeedIsZero* UAT_WaitForSpeedIsZero::CreateWaitForZeroSpeed(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAT_WaitForSpeedIsZero* MyObj = NewAbilityTask<UAT_WaitForSpeedIsZero>(OwningAbility, TaskInstanceName);

	return MyObj;
}

void UAT_WaitForSpeedIsZero::Activate()
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo)
	{
		Cast<APlayerBase>(ActorInfo->AvatarActor)->OnZeroSpeed.AddUObject(this, &UAT_WaitForSpeedIsZero::OnSpeedIsNearToZero);
	}
}

void UAT_WaitForSpeedIsZero::OnSpeedIsNearToZero()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnNearToZero.Broadcast();
	}

	EndTask();
}

void UAT_WaitForSpeedIsZero::OnDestroy(bool AbilityIsEnding)
{
	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo)
	{
		Cast<APlayerBase>(ActorInfo->AvatarActor)->OnZeroSpeed.Clear();
	}

	Super::OnDestroy(AbilityIsEnding);
}