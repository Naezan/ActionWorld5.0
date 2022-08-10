// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ActionWorldRPGGameModeBase.generated.h"

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API AActionWorldRPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	AActionWorldRPGGameModeBase();

	//Begin GameModeBase interface
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	//End GameModeBase interface

	void UpdateLoadState();

	void PlayerDied(AController* Controller);

protected:
	float RespawnDelay;

	TSubclassOf<class AActionCharacterBase> HeroClass;

	AActor* EnemySpawnPoint;

	virtual void BeginPlay() override;

	void RespawnPlayer(AController* Controller);
};
