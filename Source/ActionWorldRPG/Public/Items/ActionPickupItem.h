// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

//게임플레이 태그
#include "GameplayTagContainer.h"

#include "ActionPickupItem.generated.h"

class AActionCharacterBase;

UCLASS()
class ACTIONWORLDRPG_API AActionPickupItem : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AActionPickupItem();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Pickup on touch
	virtual void NotifyActorBeginOverlap(class AActor* Other) override;

	virtual bool CanBePickedUp(AActionCharacterBase* Acquirer) const;

	UFUNCTION(BlueprintNativeEvent, Meta = (DisplayName = "CanBePickedUp"))
		bool K2_CanBePickedUp(AActionCharacterBase* Acquirer) const;
	virtual bool K2_CanBePickedUp_Implementation(AActionCharacterBase* Acquirer) const;

protected:
	UFUNCTION()
	void PickupOnCollision(AActionCharacterBase* Acquirer);

	//Apply Gameplay Ability or Effect
	UFUNCTION()
	virtual void GivePickUpTo(AActionCharacterBase* Acquirer);

	UFUNCTION()
	virtual void OnPickedUp();

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnPickedUp"))
		void K2_OnPickedUp();

	UFUNCTION()
	virtual void OnRespawned();

	UFUNCTION(BlueprintImplementableEvent, Meta = (DisplayName = "OnRespawned"))
		void K2_OnRespawned();

	UFUNCTION()
	virtual void RespawnPickup();

	UFUNCTION()
	virtual void OnRep_IsActive();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PickUp")
		class UCapsuleComponent* CollisionComp;

	UPROPERTY(BlueprintReadOnly, Replicated)
		AActionCharacterBase* PickedUpBy;

	//Respawn
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsActive, Category = "PickUp")
		bool bIsActive;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PickUp")
		bool bCanRespawn;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PickUp")
		float RespawnTime;

	//GameplayAbility
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PickUp")
		FGameplayTagContainer RestrictedPickupTags;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PickUp")
		TArray<TSubclassOf<class UActionGameplayAbility>> GrantedAbility;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "PickUp")
		TArray<TSubclassOf<class UGameplayEffect>> GrantedEffect;

	//Sound
	UPROPERTY(EditDefaultsOnly, Category = "PickUp")
		class USoundCue* PickUpSound;

	FTimerHandle TimerHandle_RespawnPickup;
};
