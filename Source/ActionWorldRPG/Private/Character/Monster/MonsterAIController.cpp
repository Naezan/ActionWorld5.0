// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MonsterAIController.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"

#include "Character/ActionCharacterBase.h"

AMonsterAIController::AMonsterAIController()
{

}

void AMonsterAIController::OnUnPossess()
{
	// Make sure the pawn that is being unpossessed doesn't remain our ASC's avatar actor
	if (APawn* PawnBeingUnpossessed = GetPawn())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PawnBeingUnpossessed))
		{
			if (ASC->GetAvatarActor() == PawnBeingUnpossessed)
			{
				ASC->SetAvatarActor(nullptr);
			}
		}
	}

	Super::OnUnPossess();
}