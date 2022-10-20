// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilitySpec.h"
#include "GameplayTagContainer.h"

#include "ActionWorldRPG/ActionWorldRPG.h"
#include "WeaponBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FWeaponAmmoChangedDelegate, int32, OldValue, int32, NewValue);

class APlayerBase;
class UAnimMontage;
class UPaperSprite;
class UActionAbilitySystemComponent;
class UActionGameplayAbility;
class USkeletalMeshComponent;
class AGATA_LineTrace;

UCLASS(Blueprintable, BlueprintType)
class ACTIONWORLDRPG_API AWeaponBase : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AWeaponBase();

	// Whether or not to spawn this weapon with collision enabled (pickup mode).
	// Set to false when spawning directly into a player's inventory or true when spawning into the world in pickup mode.
	UPROPERTY(BlueprintReadWrite)
		bool bSpawnWithCollision;

	// This tag is primarily used by the first person Animation Blueprint to determine which animations to play
	// (Rifle vs Rocket Launcher)
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
		FGameplayTag WeaponTag;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
		FGameplayTagContainer RestrictedPickupTags;

	// UI HUD Primary Icon when equipped. Using Sprites because of the texture atlas from ShooterGame.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
		UPaperSprite* PrimaryIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
		UPaperSprite* SecondaryIcon;

	// UI HUD Primary Clip Icon when equipped
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
		UPaperSprite* PrimaryClipIcon;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
		UPaperSprite* SecondaryClipIcon;

	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Weapon")
		FGameplayTag FireMode;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
		FGameplayTag PrimaryAmmoType;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Weapon")
		FGameplayTag SecondaryAmmoType;

	// Things like fire mode for rifle
	UPROPERTY(BlueprintReadWrite, VisibleInstanceOnly, Category = "Weapon")
		FText StatusText;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
		FWeaponAmmoChangedDelegate OnPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
		FWeaponAmmoChangedDelegate OnMaxPrimaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
		FWeaponAmmoChangedDelegate OnSecondaryClipAmmoChanged;

	UPROPERTY(BlueprintAssignable, Category = "Weapon")
		FWeaponAmmoChangedDelegate OnMaxSecondaryClipAmmoChanged;

	// Implement IAbilitySystemInterface
	virtual class UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Weapon")
		virtual USkeletalMeshComponent* GetWeaponMesh3P() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker) override;

	void SetOwningCharacter(APlayerBase* InOwningCharacter);

	// 무기 상호작용시 호출됩니다.
	virtual void AddWeaponInfoOnInteract(class AActor* Other);

	// Called when the player equips this weapon
	virtual void Equip();

	// Called when the player unequips this weapon
	virtual void UnEquip();

	virtual void AddAbilities();

	virtual void RemoveAbilities();

	virtual int32 GetAbilityLevel(EActionAbilityInputID AbilityID);

	// Resets things like fire mode to default
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void ResetWeapon();

	UFUNCTION(NetMulticast, Reliable)
		void OnDropped(FVector NewLocation);
	virtual void OnDropped_Implementation(FVector NewLocation);
	virtual bool OnDropped_Validate(FVector NewLocation);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual int32 GetPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual int32 GetMaxPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual int32 GetSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual int32 GetMaxSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual void SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo);

	//UFUNCTION(BlueprintCallable, Category = "Weapon")
	//	TSubclassOf<class UGSHUDReticle> GetPrimaryHUDReticleClass() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		virtual bool HasInfiniteAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Animation")
		UAnimMontage* GetEquip3PMontage() const;

	UFUNCTION(BlueprintCallable, Category = "Audio")
		class USoundCue* GetPickupSound() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
		FText GetDefaultStatusText() const;

	// Getter for LineTraceTargetActor. Spawns it if it doesn't exist yet.
	UFUNCTION(BlueprintCallable, Category = "Targeting")
		AGATA_LineTrace* GetLineTraceTargetActor();

	// Getter for SphereTraceTargetActor. Spawns it if it doesn't exist yet.
	//UFUNCTION(BlueprintCallable, Category = "GASShooter|Targeting")
	//	AGATA_SphereTrace* GetSphereTraceTargetActor();

protected:
	UPROPERTY()
		UActionAbilitySystemComponent* AbilitySystemComponent;

	// How much ammo in the clip the gun starts with
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_PrimaryClipAmmo, Category = "Weapon|Ammo")
		int32 PrimaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxPrimaryClipAmmo, Category = "Weapon|Ammo")
		int32 MaxPrimaryClipAmmo;

	// How much ammo in the clip the gun starts with. Used for things like rifle grenades.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_SecondaryClipAmmo, Category = "Weapon|Ammo")
		int32 SecondaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_MaxSecondaryClipAmmo, Category = "Weapon|Ammo")
		int32 MaxSecondaryClipAmmo;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon|Ammo")
		bool bInfiniteAmmo;

	//UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "UI")
	//	TSubclassOf<class UGSHUDReticle> PrimaryHUDReticleClass;

	UPROPERTY()
		AGATA_LineTrace* LineTraceTargetActor;

	// Collision capsule for when weapon is in pickup mode
	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
		USkeletalMeshComponent* WeaponMesh3P;

	// Relative Location of weapon 3P Mesh when in pickup mode
	// 1P weapon mesh is invisible so it doesn't need one
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FVector WeaponMesh3PickupRelativeLocation;

	// Relative Location of weapon 3P Mesh when equipped
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
		FVector WeaponMesh3PEquippedRelativeLocation;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Weapon")
		APlayerBase* OwningCharacter;

	UPROPERTY(EditAnywhere, Category = "Weapon")
		TArray<TSubclassOf<UActionGameplayAbility>> Abilities;

	UPROPERTY(BlueprintReadOnly, Category = "Weapon")
		TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Weapon")
		FGameplayTag DefaultFireMode;

	// Things like fire mode for rifle
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Weapon")
		FText DefaultStatusText;

	UPROPERTY(BlueprintReadonly, EditAnywhere, Category = "Animation")
		UAnimMontage* Equip3PMontage;

	// Sound played when player picks it up
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
		class USoundCue* PickupSound;

	// Cache tags
	FGameplayTag WeaponPrimaryInstantAbilityTag;
	FGameplayTag WeaponSecondaryInstantAbilityTag;
	FGameplayTag WeaponAlternateInstantAbilityTag;
	FGameplayTag WeaponIsFiringTag;

	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	// Called when the player picks up this weapon
	virtual void PickUpOnInteract(APlayerBase* InCharacter);

	UFUNCTION()
		virtual void OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo);

	UFUNCTION()
		virtual void OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo);

	UFUNCTION()
		virtual void OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo);

	UFUNCTION()
		virtual void OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo);

public:
	/*
	* Textures for the weapon cosshairs
	*/
	UPROPERTY(EditAnywhere, Category = Crosshairs)
		TObjectPtr<class UTexture2D> CrosshairsCenter;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		TObjectPtr<class UTexture2D> CrosshairsLeft;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		TObjectPtr<class UTexture2D> CrosshairsRight;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		TObjectPtr<class UTexture2D> CrosshairsTop;

	UPROPERTY(EditAnywhere, Category = Crosshairs)
		TObjectPtr<class UTexture2D> CrosshairsBottom;

	/*
	* Aiming ZoomFOV
	*/
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		float ZoomFOV = 30.f;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Camera")
		float ZoomDuration = 0.1f;
};
