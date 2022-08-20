// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LoadingScreenInterface.h"

#include "ActionGameState.generated.h"

class UBOutExperienceManagerComponent;

enum class EActionLoadState
{
	Unloaded,
	Loading, 
	LoadingGameFeatures,
	ExcutingGameFeatureActions,
	Loaded,
	Deactivated,
};

UCLASS()
class ACTIONWORLDRPG_API AActionGameState : public AGameStateBase, public ILoadingScreenInterface
{
	GENERATED_BODY()

public:
	AActionGameState();

	//Begin Actor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//End Actor interface

	//Begin GameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	//End GameStateBase interface

	//~ILoadingProcessInterface interface
	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;
	//~End of ILoadingProcessInterface

	void SetLoadState(EActionLoadState NewLoadState);
	EActionLoadState GetLoadState() const;

	void StartLoadGame();
	void LoadCompleteGame();

	void OnExperienceFullLoadCompleted();
	void OnAllActionsDeactivated();

private:
	UPROPERTY()
	UBOutExperienceManagerComponent* ExperienceManagerComponent;
	//UPROPERTY()
	//GameStateComponent
	EActionLoadState LoadState = EActionLoadState::Unloaded;
};
