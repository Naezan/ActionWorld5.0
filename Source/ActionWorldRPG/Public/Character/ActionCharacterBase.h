// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../../ActionWorldRPG.h"
//¾îºô¸®Æ¼
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/PlayerAttributeSet.h"
#include "ActionCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AActionCharacterBase*, Character);

//¾îºô¸®Æ¼
class UAbilitySystemComponent;
class UActionAbilitySystemComponent;
class UGameplayEffect;
class UActionGameplayAbility;

USTRUCT(BlueprintType)
struct ACTIONWORLDRPG_API FActionDamageNumber
{
	GENERATED_USTRUCT_BODY()

		float DamageAmount;

	FGameplayTagContainer Tags;

	FActionDamageNumber() {}

	FActionDamageNumber(float InDamageAmount, FGameplayTagContainer InTags) : DamageAmount(InDamageAmount)
	{
		// Copy tag container
		Tags.AppendTags(InTags);
	}
};

UCLASS()
class ACTIONWORLDRPG_API AActionCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacterBase();

	// Friended to allow access to handle functions above
	friend UPlayerAttributeSet;

	UPROPERTY(BlueprintAssignable, Category = "Character")
		FCharacterDiedDelegate OnCharacterDied;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	class UPlayerAttributeSet* GetPlayerAttributeSet() const;

	class UAmmoAttributeSet* GetAmmoAttributeSet() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual bool IsAlive() const;

	// Switch on AbilityID to return individual ability levels.
	UFUNCTION(BlueprintCallable, Category = "Character")
	virtual int32 GetAbilityLevel(EActionAbilityInputID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();

	virtual void Die();

	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual void FinishDying();

	virtual void AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags);



	//===========================
	// Get Attributes
	//===========================

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMaxShield() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		float GetMoveSpeedBaseValue() const;

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	TArray<FActionDamageNumber> DamageNumberQueue;
	FTimerHandle DamageNumberTimer;

	/** The component used to handle ability system interactions */
	UActionAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
	UPlayerAttributeSet* PlayerAttributeSet;

	UPROPERTY()
	UAmmoAttributeSet* AmmoAttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
		FText CharacterName;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* DeathMontage;

	// Default abilities for this Character. These will be removed on Character death and regiven if Character respawns.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		TArray<TSubclassOf<UActionGameplayAbility>> CharacterAbilities;

	// Default attributes for a character for initializing on spawn/respawn.
	// This is an instant GE that overrides the values for attributes that get reset on spawn/respawn.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		TSubclassOf<UGameplayEffect> DefaultAttributes;

	// These effects are only applied one time on startup
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Abilities")
		TArray<TSubclassOf<UGameplayEffect>> StartupEffects;

	//UPROPERTY(EditAnywhere, Category = "UI")
	//	TSubclassOf<class UGSDamageTextWidgetComponent> DamageNumberClass;

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void ShowDamageNumber();

	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
	virtual void SetShield(float Shield);
};
