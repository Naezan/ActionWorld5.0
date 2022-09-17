// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "AbilitySystemInterface.h"
#include "Interfaces/ItemSpawnableInterface.h"

#include "InteractableActor.generated.h"

UCLASS()
class ACTIONWORLDRPG_API AInteractableActor : public AActor, public IAbilitySystemInterface, public IItemSpawnableInterface
{
	GENERATED_BODY()

public:
	AInteractableActor();

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void PreInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY()
		class UActionAbilitySystemComponent* AbilitySystemComponent;

	//MemberFunction
public:

protected:
	//PreInteract에서 호출됩니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void InteractWithInstigator(AActor* Interactor);
	virtual void InteractWithInstigator_Implementation(AActor* Interactor);

	//PostInteract에서 호출됩니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void InteractDelayWithInstigator(AActor* Interactor);
	virtual void InteractDelayWithInstigator_Implementation(AActor* Interactor);

	//PostInteract에서 호출됩니다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void SendToInstigator(AActor* Interactor);
	virtual void SendToInstigator_Implementation(AActor* Interactor);

	//MemberVariable
public:

protected:
};
