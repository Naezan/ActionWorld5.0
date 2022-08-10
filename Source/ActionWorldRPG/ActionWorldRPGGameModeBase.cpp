// Copyright Epic Games, Inc. All Rights Reserved.


#include "ActionWorldRPGGameModeBase.h"

#include "Character/ActionCharacterBase.h"
#include "Character/Player/ActionPlayerController.h"

#include "ActionGameState.h"

#include "GameFramework/SpectatorPawn.h"
#include "Kismet/GameplayStatics.h"

AActionWorldRPGGameModeBase::AActionWorldRPGGameModeBase()
{
	RespawnDelay = 5.0f;

	HeroClass = StaticLoadClass(UObject::StaticClass(), nullptr, TEXT("/Game/Characters/Heroes/BP_Player.BP_Player_C"));
	if (!HeroClass)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Failed to find HeroClass. If it was moved, please update the reference location in C++."), *FString(__FUNCTION__));
	}
}

void AActionWorldRPGGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	GEngine->AddOnScreenDebugMessage(99, 5, FColor::Blue, TEXT("InitGame"));

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::UpdateLoadState);
}

void AActionWorldRPGGameModeBase::UpdateLoadState()
{
	AActionGameState* ActionGameState = Cast<AActionGameState>(GameState);
	ActionGameState->StartLoadGame();
}

void AActionWorldRPGGameModeBase::PlayerDied(AController* Controller)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	ASpectatorPawn* SpectatorPawn = GetWorld()->SpawnActor<ASpectatorPawn>(SpectatorClass, Controller->GetPawn()->GetActorTransform(), SpawnParameters);

	Controller->UnPossess();
	Controller->Possess(SpectatorPawn);

	FTimerHandle RespawnTimerHandle;
	FTimerDelegate RespawnDelegate;

	RespawnDelegate = FTimerDelegate::CreateUObject(this, &AActionWorldRPGGameModeBase::RespawnPlayer, Controller);
	GetWorldTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, RespawnDelay, false);

	AActionPlayerController* PC = Cast<AActionPlayerController>(Controller);
	if (PC)
	{
		//PC->SetRespawnCountdown(RespawnDelay);
	}
}

void AActionWorldRPGGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	//// Get the enemy hero spawn point
	//TArray<AActor*> Actors;
	//UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), Actors);
	//for (AActor* Actor : Actors)
	//{
	//	if (Actor->GetName() == FString("EnemyHeroSpawn"))
	//	{
	//		EnemySpawnPoint = Actor;
	//		break;
	//	}
	//}

	//if (!EnemySpawnPoint)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("%s EnemySpawnPoint is null."), *FString(__FUNCTION__));
	//}
}

void AActionWorldRPGGameModeBase::RespawnPlayer(AController* Controller)
{
	if (!IsValid(Controller))
	{
		return;
	}

	if (Controller->IsPlayerController())
	{
		// Respawn player hero
		AActor* PlayerStart = FindPlayerStart(Controller);

		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActionCharacterBase* Hero = GetWorld()->SpawnActor<AActionCharacterBase>(HeroClass, PlayerStart->GetActorLocation(), PlayerStart->GetActorRotation(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);

		AActionPlayerController* PC = Cast<AActionPlayerController>(Controller);
		if (PC)
		{
			//PC->ClientSetControlRotation(PlayerStart->GetActorRotation());
		}
	}
	else
	{
		// Respawn AI hero
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AActionCharacterBase* Hero = GetWorld()->SpawnActor<AActionCharacterBase>(HeroClass, EnemySpawnPoint->GetActorTransform(), SpawnParameters);

		APawn* OldSpectatorPawn = Controller->GetPawn();
		Controller->UnPossess();
		OldSpectatorPawn->Destroy();
		Controller->Possess(Hero);
	}
}
