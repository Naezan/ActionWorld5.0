// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ActionCharacterBase.h"

//입력
#include "ActionWorldRPG/ActionWorldRPG.h"

//UI
#include "HUD/PlayerHUD.h"

//인터페이스
#include "Interfaces/InteractCrosshairInterface.h"

#include "PlayerBase.generated.h"

//카메라
class UCameraComponent;
class USpringArmComponent;

class AWeaponBase;
class UInputAction;

//어빌리티
UENUM(BlueprintType)
enum class EPlayerWeaponState : uint8
{
	// 0
	Rifle					UMETA(DisplayName = "Rifle"),
	// 1
	RifleAiming				UMETA(DisplayName = "Rifle Aiming"),
	// 2
	RocketLauncher			UMETA(DisplayName = "Rocket Launcher"),
	// 3
	RocketLauncherAiming	UMETA(DisplayName = "Rocket Launcher Aiming")
};

USTRUCT()
struct ACTIONWORLDRPG_API FPlayerInventory
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY()
		TArray<AWeaponBase*> Weapons;

	// Consumable items

	// Passive items like armor

	// Door keys

	// Etc
};

UCLASS()
class ACTIONWORLDRPG_API APlayerBase : public AActionCharacterBase, public IInteractCrosshairInterface
{
	GENERATED_BODY()

public:
	APlayerBase();

	//어빌리티 관련프로퍼티
	FGameplayTag CurrentWeaponTag;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	//넉다운 이벤트

	virtual void FinishDying() override;

	//3인칭 프로퍼티
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual bool IsInFirstPersonPerspective() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
		USkeletalMeshComponent* GetThirdPersonMesh() const;


	//=========== Weapon ============
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		AWeaponBase* GetCurrentWeapon() const;

	// Adds a new weapon to the inventory.
// Returns false if the weapon already exists in the inventory, true if it's a new weapon.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool AddWeaponToInventory(AWeaponBase* NewWeapon, bool bEquipWeapon = false);

	// Removes a weapon from the inventory.
	// Returns true if the weapon exists and was removed. False if the weapon didn't exist in the inventory.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
		bool RemoveWeaponFromInventory(AWeaponBase* WeaponToRemove);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void RemoveAllWeaponsFromInventory();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		void EquipWeapon(AWeaponBase* NewWeapon);

	UFUNCTION(Server, Reliable)
		void ServerEquipWeapon(AWeaponBase* NewWeapon);
	void ServerEquipWeapon_Implementation(AWeaponBase* NewWeapon);
	bool ServerEquipWeapon_Validate(AWeaponBase* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void NextWeapon();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		virtual void PreviousWeapon();

	FName GetWeaponAttachPoint();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetMaxPrimaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetPrimaryReserveAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetMaxSecondaryClipAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetSecondaryReserveAmmo() const;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
		int32 GetNumWeapons() const;

	//HUD
	UFUNCTION(BlueprintCallable, Category = "HUD")
		FVector GetHitTarget() const;
	UFUNCTION(BlueprintCallable, Category = "HUD")
		void SetHitTarget(FVector hitTarget);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PostInitializeComponents() override;

	virtual void Tick(float DeltaTime) override;

	// =========== Enhanced Input ===========
	void HandleWeaponPrimaryActionPressed();
	void HandleWeaponPrimaryActionReleased();
	void HandleWeaponSecondaryActionPressed();
	void HandleWeaponSecondaryActionReleased();
	void HandleWeaponAlternateActionPressed();
	void HandleWeaponAlternateActionReleased();
	void HandleReloadActionPressed();
	void HandleReloadActionReleased();
	void HandleNextWeaponActionPressed();
	void HandleNextWeaponActionReleased();
	void HandlePreviousWeaponActionPressed();
	void HandlePreviousWeaponActionReleased();

	void CallGenericConfirm();
	void CallGenericCancel();

	void SendLocalInputToASC(bool bIsPressed, const EActionAbilityInputID AbilityInputID);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> GenericConfirmAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> GenericCancelAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> EquipPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> EquipSecondaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> WeaponPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> WeaponSecondaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> WeaponAlternateAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> NextWeaponAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> PreviousWeaponAction;

	//=============================



	// Toggles between perspectives
	void TogglePerspective();

	// Sets the perspective
	void SetPerspective(bool Is1PPerspective);

	// Client only
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		FVector StartingThirdPersonMeshLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		bool bIsFirstPersonPerspective;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		bool bIsAiming = false;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		bool bIsReloading = false;

	// Set to true when we change the weapon predictively and flip it to false when the Server replicates to confirm.
	// We use this if the Server refused a weapon change ability's activation to ask the Server to sync the client back up
	// with the correct CurrentWeapon.
	bool bChangedWeaponLocally;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Default3PFOV;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
		FName WeaponAttachPoint;

	//컨트롤러
	class AActionPlayerController* PlayerController;

	//HUD
	class APlayerHUD* PlayerHUD;
	FHUDPackage HUDPackage;
	FVector HitTarget;

	float CrosshairVelocityFactor; //뛰거나 걸을때
	float CrosshairInAirFactor; //공중에 있을때
	float CrosshairAimFactor; //에임동작일때
	float CrosshairShootingFactor; //총을 쏠때

	//카메라
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
		USpringArmComponent* CameraSpringArmComp;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
		UCameraComponent* CameraComp;

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
		FPlayerInventory Inventory;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
		TArray<TSubclassOf<AWeaponBase>> DefaultInventoryWeaponClasses;

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon)
		AWeaponBase* CurrentWeapon;

	//UPROPERTY()
	//class UAmmoAttributeSet* AmmoAttributeSet;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
		TSubclassOf<UGameplayEffect> DeathEffect;

	// Cache tags
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	// Attribute changed delegate handles
	FDelegateHandle PrimaryReserveAmmoChangedDelegateHandle;
	FDelegateHandle SecondaryReserveAmmoChangedDelegateHandle;

	// Tag changed delegate handles
	FDelegateHandle WeaponChangingDelayReplicationTagChangedDelegateHandle;

	// Server spawns default inventory
	void SpawnDefaultInventory();

	void SetupStartupPerspective();

	bool DoesWeaponExistInInventory(AWeaponBase* InWeapon);

	void SetCurrentWeapon(AWeaponBase* NewWeapon, AWeaponBase* LastWeapon);

	// Unequips the specified weapon. Used when OnRep_CurrentWeapon fires.
	void UnEquipWeapon(AWeaponBase* WeaponToUnEquip);

	// Unequips the current weapon. Used if for example we drop the current weapon.
	void UnEquipCurrentWeapon();

	UFUNCTION()
		virtual void CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo);

	UFUNCTION()
		virtual void CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo);

	// Attribute changed callbacks
	virtual void CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data);
	virtual void CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data);

	// Tag changed callbacks
	virtual void WeaponChangingDelayReplicationTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UFUNCTION()
		void OnRep_CurrentWeapon(AWeaponBase* LastWeapon);

	UFUNCTION()
		void OnRep_Inventory();

	void OnAbilityActivationFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailTags);

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// The autonomous client can use this to request the proper CurrentWeapon from the server when it knows it may be
	// out of sync with it from predictive client-side changes.
	UFUNCTION(Server, Reliable)
		void ServerSyncCurrentWeapon();
	void ServerSyncCurrentWeapon_Implementation();
	bool ServerSyncCurrentWeapon_Validate();

	// The CurrentWeapon is only automatically replicated to simulated clients.
	// Use this function to manually sync the autonomous client's CurrentWeapon when we're ready to.
	// This allows us to predict weapon changes (changing weapons fast multiple times in a row so that the server doesn't
	// replicate and clobber our CurrentWeapon).
	UFUNCTION(Client, Reliable)
		void ClientSyncCurrentWeapon(AWeaponBase* InWeapon);
	void ClientSyncCurrentWeapon_Implementation(AWeaponBase* InWeapon);
	bool ClientSyncCurrentWeapon_Validate(AWeaponBase* InWeapon);

	/*
	* HUD
	*/
	void SetHUDCrosshairs(float DeltaTime);
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	/*
	* ZoomFOV Aiming
	*/
	void InterpFOV(float DeltaTime);
};
