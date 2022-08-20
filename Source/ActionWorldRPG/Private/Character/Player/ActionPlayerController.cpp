// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/ActionPlayerController.h"
#include "Character/Player/PlayerBase.h"

void AActionPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AActionPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AActionPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AActionPlayerController::SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite)
{
}

void AActionPlayerController::SetEquippedWeaponStatusText(const FText& StatusText)
{
}

void AActionPlayerController::SetPrimaryClipAmmo(int32 ClipAmmo)
{
}

void AActionPlayerController::SetPrimaryReserveAmmo(int32 ReserveAmmo)
{
}

void AActionPlayerController::SetSecondaryClipAmmo(int32 SecondaryClipAmmo)
{
}

void AActionPlayerController::SetSecondaryReserveAmmo(int32 SecondaryReserveAmmo)
{
}

void AActionPlayerController::ShowDamageNumber_Implementation(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags)
{
	if (IsValid(TargetCharacter))
	{
		TargetCharacter->AddDamageNumber(DamageAmount, DamageNumberTags);
	}
}

bool AActionPlayerController::ShowDamageNumber_Validate(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags)
{
	return true;
}

void AActionPlayerController::SetRespawnCountdown_Implementation(float RespawnTimeRemaining)
{
}

bool AActionPlayerController::SetRespawnCountdown_Validate(float RespawnTimeRemaining)
{
	return true;
}

void AActionPlayerController::ClientSetControlRotation_Implementation(FRotator NewRotation)
{
	SetControlRotation(NewRotation);
}

bool AActionPlayerController::ClientSetControlRotation_Validate(FRotator NewRotation)
{
	return true;
}

void AActionPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//AGSPlayerState* PS = GetPlayerState<AGSPlayerState>();
	//if (PS)
	//{
	//    // Init ASC with PS (Owner) and our new Pawn (AvatarActor)
	//    PS->GetAbilitySystemComponent()->InitAbilityActorInfo(PS, InPawn);
	//}
}

void AActionPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

void AActionPlayerController::Kill()
{
	ServerKill();
}

void AActionPlayerController::ServerKill_Implementation()
{
	APlayerBase* PC = GetOwner<APlayerBase>();
	if (PC)
	{
		PC->GetDefaultAttributeSet()->SetHealth(0.f);
	}
}

bool AActionPlayerController::ServerKill_Validate()
{
	return true;
}
