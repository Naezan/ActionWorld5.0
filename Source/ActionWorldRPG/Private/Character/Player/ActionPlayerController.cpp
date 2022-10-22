// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/ActionPlayerController.h"
#include "Character/Player/PlayerBase.h"
#include "HUD/InteractionHUD.h"

void AActionPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AActionPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	CreateInteractionHUD();
}

void AActionPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AActionPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AActionPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
}

void AActionPlayerController::CreateInteractionHUD()
{
	if (InteractionWidget)
	{
		return;
	}

	if (!InteractionWidgetClass)
	{
		return;
	}

	if (!IsLocalPlayerController())
	{
		return;
	}

	InteractionWidget = CreateWidget<UInteractionHUD>(this, InteractionWidgetClass);
	InteractionWidget->AddToViewport();
}

UInteractionHUD* AActionPlayerController::GetInteractionHUD()
{
	return InteractionWidget;
}

UUserWidget* AActionPlayerController::GetBlackOutWidget()
{
	return BlackOutWidget;
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
