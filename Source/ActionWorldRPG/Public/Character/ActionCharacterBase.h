// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "../../ActionWorldRPG.h"

//어빌리티
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/PlayerAttributeSet.h"

//팀
#include "GenericTeamAgentInterface.h"

#include "ActionCharacterBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCharacterDiedDelegate, AActionCharacterBase*, Character);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHealthAttributeChanged, AActionCharacterBase*, AttributeOwner, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FManaAttributeChanged, AActionCharacterBase*, AttributeOwner, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRifleAmmoAttributeChanged, AActionCharacterBase*, AttributeOwner, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FPistolAmmoAttributeChanged, AActionCharacterBase*, AttributeOwner, float, OldValue, float, NewValue);

//어빌리티
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
class ACTIONWORLDRPG_API AActionCharacterBase : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AActionCharacterBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;

	// Friended to allow access to handle functions above
	friend UPlayerAttributeSet;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	// Client only
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UFUNCTION(BlueprintCallable, Category = "Character")
		UActionAbilitySystemComponent* GetActionAbilitySystemComponent() const;
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	//AttributeSet
	class UPlayerAttributeSet* GetDefaultAttributeSet() const;

	class UAmmoAttributeSet* GetAmmoAttributeSet() const;

	//===========================
	// Ability
	//===========================
	// Switch on AbilityID to return individual ability levels.
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual int32 GetAbilityLevel(EActionAbilityInputID AbilityID) const;

	// Removes all CharacterAbilities. Can only be called by the Server. Removing on the Server will remove from Client too.
	virtual void RemoveCharacterAbilities();

	UFUNCTION(BlueprintCallable, Category = "Abilities")
		bool ActivateAbilityWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation = true);

	//===========================
	// Death
	//===========================
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual bool IsAlive() const;

	virtual void StartDeath();
	virtual void FinishDeath();
	
	//===========================
	// UI
	//===========================
	virtual void ShowDamageNumber();
	virtual void AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags);

	//===========================
	// Get Attributes
	//===========================

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual int32 GetCharacterLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxMana() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxStamina() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetShield() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMaxShield() const;

	// Gets the Current value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMoveSpeed() const;

	// Gets the Base value of MoveSpeed
	UFUNCTION(BlueprintCallable, Category = "Attributes")
		virtual float GetMoveSpeedBaseValue() const;

public:
	//===========================
	// Delegate
	//===========================

	//블루프린트용
	UPROPERTY(BlueprintAssignable)
		FCharacterDiedDelegate OnDeathStarted;
	UPROPERTY(BlueprintAssignable)
		FCharacterDiedDelegate OnDeathFinished;

	UPROPERTY(BlueprintAssignable)
		FHealthAttributeChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable)
		FManaAttributeChanged OnManaChanged;
	UPROPERTY(BlueprintAssignable)
		FRifleAmmoAttributeChanged OnRifleAmmoChanged;
	UPROPERTY(BlueprintAssignable)
		FPistolAmmoAttributeChanged OnPistolAmmoChanged;

protected:
	UFUNCTION()
		virtual void OnInitializeAbilitySystem();
	UFUNCTION()
		virtual void UnitializeAbilitySystem();

	// Grant abilities on the Server. The Ability Specs will be replicated to the owning client.
	virtual void AddCharacterAbilities();

	// Initialize the Character's attributes. Must run on Server but we run it on Client too
	// so that we don't have to wait. The Server's replication to the Client won't matter since
	// the values should be the same.
	virtual void InitializeAttributes();

	virtual void AddStartupEffects();

	virtual void HandleZeroHealth(AActor* Instigator, const FGameplayEffectSpec& DamageEffectSpec, float DamageAmount);
	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleManaChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandleRifleReserveAmmoChanged(const FOnAttributeChangeData& ChangeData);
	virtual void HandlePistolReserveAmmoChanged(const FOnAttributeChangeData& ChangeData);

	virtual void SetHealth(float Health);
	virtual void SetMana(float Mana);
	virtual void SetStamina(float Stamina);
	virtual void SetShield(float Shield);

	void DisableMovementAndCollision();
	void PlayDeathAnimationAndSound();
	/*
	* 차후에 다른방식으로써야한다 그냥렉돌만작동하는 몬스터가 있을수도있고 애니메이션이적영된후에 렉돌이 작동할 수도 있다.
	*/
	UFUNCTION(BlueprintCallable)
	void Ragdoll();
	void ApplyPhysics();
	void HideEquippedWeapon();
	void OnDestroyFinished();
	void DestroyWhenFinishDeath();
	virtual void FinishDying();

	//블루프린트용
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDestroyStarted"))
		void K2_OnDestroyStarted();
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "OnDestroyFinished"))
		void K2_OnDestroyFinished();

protected:
	FGameplayTag DeadTag;
	FGameplayTag EffectRemoveOnDeathTag;

	TArray<FActionDamageNumber> DamageNumberQueue;
	FTimerHandle DamageNumberTimer;

	/** The component used to handle ability system interactions */
	UActionAbilitySystemComponent* AbilitySystemComponent;

	/** List of attributes modified by the ability system */
	UPROPERTY()
		UPlayerAttributeSet* DefaultAttributeSet;

	UPROPERTY()
		UAmmoAttributeSet* AmmoAttributeSet;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
		FText CharacterName;

	// Melee Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* MeleeMontage;

	// Hit Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		UAnimMontage* HitMontage;

	// Death Animation
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Animation")
		TArray<UAnimMontage*> DeathMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Audio")
		class USoundCue* DeathSound;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ragdoll")
		FName RagdollProfileName;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ragdoll")
		FName RagdollImpulseBone;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ragdoll")
		float RadgollImpulsePower;

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

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
		TSubclassOf<UGameplayEffect> DeathEffect;

	//UPROPERTY(EditAnywhere, Category = "UI")
	//	TSubclassOf<class UGSDamageTextWidgetComponent> DamageNumberClass;


	//TeamID
public:
	//~ILyraTeamAgentInterface interface
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;

	/** Returns the Team ID of the team the player belongs to. */
	UFUNCTION(BlueprintCallable)
		int32 GetTeamId() const
	{
		return (MyTeamID == FGenericTeamId::NoTeam) ? INDEX_NONE : (int32)MyTeamID;
	}

private:
	UPROPERTY()
		FGenericTeamId MyTeamID;
};
