// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/ActionPlayerController.h"

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
}

bool AActionPlayerController::ShowDamageNumber_Validate(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags)
{
    return false;
}
