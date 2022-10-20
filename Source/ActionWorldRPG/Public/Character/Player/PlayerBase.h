// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ActionCharacterBase.h"

//�Է�
#include "ActionWorldRPG/ActionWorldRPG.h"

//UI
#include "HUD/PlayerHUD.h"

//�������̽�
#include "Interfaces/InteractCrosshairInterface.h"
#include "Interfaces/InventoryInterface.h"

#include "PlayerBase.generated.h"

//ī�޶�
class UCameraComponent;
class USpringArmComponent;

class AWeaponBase;
class UInputAction;

class UQuestWidget;
class UQuestSystemComponent;

//�����Ƽ
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

UCLASS()
class ACTIONWORLDRPG_API APlayerBase : public AActionCharacterBase, public IInteractCrosshairInterface
{
	GENERATED_BODY()

public:
	APlayerBase(const FObjectInitializer& ObjectInitializer);

	//�����Ƽ ����������Ƽ
	FGameplayTag CurrentWeaponTag;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Client only
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Controller() override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	//�˴ٿ� �̺�Ʈ

	virtual void FinishDying() override;

	//3��Ī ������Ƽ
	UFUNCTION(BlueprintCallable, Category = "Character")
		virtual bool IsInFirstPersonPerspective() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Character")
		USkeletalMeshComponent* GetThirdPersonMesh() const;

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

	FName GetWeaponAttachPoint();

	//HUD
	UFUNCTION(BlueprintCallable, Category = "HUD")
		FVector GetHitTarget() const;
	UFUNCTION(BlueprintCallable, Category = "HUD")
		void SetHitTarget(FVector hitTarget);

	//Quest UI
	UFUNCTION(BlueprintPure)
		FORCEINLINE UQuestSystemComponent* GetQuestComponent() const { return QuestComponent; };

	UFUNCTION()
	void FindQuestComponent();

	//FootStep
	UFUNCTION(BlueprintCallable, Category = "Footstep")
		EPhysicalSurface GetSurfaceType();

	//Climb
	UFUNCTION(BlueprintPure)
		FORCEINLINE class UCustomCharacterMovementComponent* GetCustomCharacterMovement() const { return MovementComponent; }

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
	void HandleEmoteActionPressed();
	void HandleEmoteActionReleased();
	void HandleInteractActionPressed();
	void HandleInteractActionReleased();
	void HandleSlidingActionPressed();
	void HandleSlidingActionReleased();

	//ClimbInput Begin
	UFUNCTION(BlueprintCallable)
		void Climb();
	UFUNCTION(BlueprintCallable)
		void CancelClimb();
	//�ϓK������ ���Ʒ��� ������ �� �����ϴ�.
	UFUNCTION(BlueprintCallable)
		void ClimbMoveForward(float Value);
	//�¿�� �������̴µ� �Ѱ谡 �����մϴ�.
	UFUNCTION(BlueprintCallable)
		void ClimbMoveRight(float Value);
	//ClimbInput End

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> EmoteAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Input Actions")
		TObjectPtr<UInputAction> SlidingAction;

	//=============================

	//ī�޶�
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
		USpringArmComponent* CameraSpringArmComp;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
		UCameraComponent* CameraComp;

	//��ǿ���
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "MotionWarp")
		class UMotionWarpingComponent* MotionWarpingComp;

	//��Ʈ�ѷ�
	UPROPERTY(BlueprintReadOnly, Category = "Character")
	class AActionPlayerController* PlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
		class UCustomCharacterMovementComponent* MovementComponent;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Character")
		FName WeaponAttachPoint;

	//������ȯ
	UPROPERTY(BlueprintReadOnly, Category = "Character")
		FVector StartingThirdPersonMeshLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Character")
		bool bIsAiming = false;
	UPROPERTY(BlueprintReadWrite, Category = "Character")
		bool bIsReloading = false;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		bool bIsFirstPersonPerspective;

	// Set to true when we change the weapon predictively and flip it to false when the Server replicates to confirm.
	// We use this if the Server refused a weapon change ability's activation to ask the Server to sync the client back up
	// with the correct CurrentWeapon.
	bool bChangedWeaponLocally;

	UPROPERTY(BlueprintReadOnly, Category = "Camera")
		float Default3PFOV;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Inventory")
		TArray<TSubclassOf<AWeaponBase>> DefaultInventoryWeaponClasses;

	//�÷��̾��� �κ��丮 �ҽ��� ���� ĳ�õ� ������
	UPROPERTY()
		TScriptInterface<IInventoryInterface> InventorySource;

	//HUD
	class APlayerHUD* PlayerHUD;
	FHUDPackage HUDPackage;
	FVector HitTarget;
	float CrosshairVelocityFactor; //�ٰų� ������
	float CrosshairInAirFactor; //���߿� ������
	float CrosshairAimFactor; //���ӵ����϶�
	float CrosshairShootingFactor; //���� ��

	//QuestUI
	UPROPERTY()
		UQuestWidget* QuestWidget;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UQuestWidget> QuestWidgetClass;
	UPROPERTY(EditAnywhere)
		UQuestSystemComponent* QuestComponent;

	//��ǿ���
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MotionWarping")
		FRotator WarpRotation;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MotionWarping")
		FVector WarpStartPoint;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MotionWarping")
		FVector WarpEdgePoint;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MotionWarping")
		FVector WarpLandPoint;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "MotionWarping")
		FVector WarpEndPoint;

	// Cache tags
	FGameplayTag NoWeaponTag;
	FGameplayTag WeaponChangingDelayReplicationTag;
	FGameplayTag WeaponAmmoTypeNoneTag;
	FGameplayTag WeaponAbilityTag;

	/** Delegate handles */

	// Attribute changed delegate handles
	FDelegateHandle PrimaryReserveAmmoChangedDelegateHandle;
	FDelegateHandle SecondaryReserveAmmoChangedDelegateHandle;

	// Tag changed delegate handles
	FDelegateHandle WeaponChangingDelayReplicationTagChangedDelegateHandle;

	//Inventory delegate handles
	FDelegateHandle InventoryUpdateHandle;
	FDelegateHandle InventoryLoadedHandle;

	// Toggles between perspectives
	void TogglePerspective();

	// Sets the perspective
	void SetPerspective(bool Is1PPerspective);

	// Server spawns default inventory
	void SpawnDefaultInventory();

	void SetupStartupPerspective();


	UFUNCTION()
		virtual void CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo);

	UFUNCTION()
		virtual void CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo);

	// Attribute changed callbacks
	virtual void CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data);
	virtual void CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data);

	UFUNCTION(BlueprintCallable)
		bool CalculateWallOverPoint(FVector AnimRootStartPoint, int32 WarpStartForwardMul = 30, int32 WarpLandForwardMul = 200, int32 WarpEndForwardMul = 50);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void StartWallOver(bool bEnable);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void EndWallOver();

	/*
	* HUD
	*/
	void SetHUDCrosshairs(float DeltaTime);
	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	//��ȣ�ۿ� UI����
	UFUNCTION(BlueprintCallable)
		void ShowInteractionPrompt(float InteractionDuration);
	//��ȣ�ۿ� UI�����
	UFUNCTION(BlueprintCallable)
		void HideInteractionPrompt();
	//��ȣ�ۿ� ������UI�����ϱ�
	UFUNCTION(BlueprintCallable)
		void StartInteractionTimer(float InteractionDuration);

	// //��ȣ�ۿ� ������UI���߰ų� ����
	UFUNCTION(BlueprintCallable)
		void StopInteractionTimer();

	/*
	* ZoomFOV Aiming
	*/
	void InterpFOV(float DeltaTime);
};
