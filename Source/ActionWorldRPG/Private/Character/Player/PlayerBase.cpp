// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/PlayerBase.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

//어빌리티
#include "AbilitySystem/Attributes/PlayerAttributeSet.h"
#include "AbilitySystem/Attributes/AmmoAttributeSet.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//카메라
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

//캐릭터 기본
#include "Engine/Classes/Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionWorldRPG/ActionWorldRPGGameModeBase.h"
#include "Character/Player/ActionPlayerController.h"

//사운드
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

//무기
#include "Weapon/WeaponBase.h"

//입력
#include "InputAction.h"
#include "EnhancedInputComponent.h"

//UI

APlayerBase::APlayerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bChangedWeaponLocally = false;
	bIsFirstPersonPerspective = false;
	Default3PFOV = 80.0f;
	NoWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.None"));
	WeaponChangingDelayReplicationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.IsChangingDelayReplication"));
	WeaponAmmoTypeNoneTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));
	WeaponAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon"));
	CurrentWeaponTag = NoWeaponTag;
	Inventory = FPlayerInventory();

	//스프링암 컴포넌트 생성
	CameraSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComp->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 108.f),
		FRotator(0.f, 0.f, 90.f));
	CameraSpringArmComp->bUsePawnControlRotation = true;
	CameraSpringArmComp->SetupAttachment(GetMesh());

	//카메라 컴포넌트 생성
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	CameraComp->SetupAttachment(CameraSpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	//캡슐컴포넌트
	GetCapsuleComponent()->InitCapsuleSize(94.f, 42.f);
	//AreaClass -> NavArea_Obstacle

	//매쉬
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f),
		FRotator(0.f, 0.f, 270.f));

	//캐릭터 무브먼트 셋팅
	GetCharacterMovement()->MaxAcceleration = 500.f;
	GetCharacterMovement()->BrakingFrictionFactor = 0.f;
	GetCharacterMovement()->GroundFriction = 4.f;
	GetCharacterMovement()->MaxWalkSpeed = 400.f;
	GetCharacterMovement()->MaxWalkSpeedCrouched = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.f;
	GetCharacterMovement()->AirControlBoostMultiplier = 0.f;
	GetCharacterMovement()->AirControlBoostVelocityThreshold = 0.f;
	GetCharacterMovement()->MaxFlySpeed = 0.f;
	GetCharacterMovement()->BrakingDecelerationFlying = 500.f;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 0.f, 0.f);
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	CrouchedEyeHeight = 19.f;
	BaseEyeHeight = 58.f;
	bUseControllerRotationYaw = false;
	bGenerateOverlapEventsDuringLevelStreaming = false;
	//SpawnCollisionHandlingMethod Always Spawn, Ignore Collisions
}

void APlayerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerBase, Inventory);
	// Only replicate CurrentWeapon to simulated clients and manually sync CurrentWeeapon with Owner when we're ready.
	// This allows us to predict weapon changing.
	DOREPLIFETIME_CONDITION(APlayerBase, CurrentWeapon, COND_SimulatedOnly);
}

void APlayerBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//InputBind
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (WeaponPrimaryAction)
		{
			EnhancedInputComponent->BindAction(WeaponPrimaryAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleWeaponPrimaryActionPressed);
			EnhancedInputComponent->BindAction(WeaponPrimaryAction, ETriggerEvent::Completed, this, &APlayerBase::HandleWeaponPrimaryActionReleased);
		}
		if (WeaponSecondaryAction)
		{
			EnhancedInputComponent->BindAction(WeaponSecondaryAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleWeaponSecondaryActionPressed);
			EnhancedInputComponent->BindAction(WeaponSecondaryAction, ETriggerEvent::Completed, this, &APlayerBase::HandleWeaponSecondaryActionReleased);
		}
		if (WeaponAlternateAction)
		{
			EnhancedInputComponent->BindAction(WeaponAlternateAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleWeaponAlternateActionPressed);
			EnhancedInputComponent->BindAction(WeaponAlternateAction, ETriggerEvent::Completed, this, &APlayerBase::HandleWeaponAlternateActionReleased);
		}
		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleReloadActionPressed);
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Completed, this, &APlayerBase::HandleReloadActionReleased);
		}
		if (NextWeaponAction)
		{
			EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleNextWeaponActionPressed);
			EnhancedInputComponent->BindAction(NextWeaponAction, ETriggerEvent::Completed, this, &APlayerBase::HandleNextWeaponActionReleased);
		}
		if (PreviousWeaponAction)
		{
			EnhancedInputComponent->BindAction(PreviousWeaponAction, ETriggerEvent::Triggered, this, &APlayerBase::HandlePreviousWeaponActionPressed);
			EnhancedInputComponent->BindAction(PreviousWeaponAction, ETriggerEvent::Completed, this, &APlayerBase::HandlePreviousWeaponActionReleased);
		}
	}
}

void APlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	WeaponChangingDelayReplicationTagChangedDelegateHandle = AbilitySystemComponent->RegisterGameplayTagEvent(WeaponChangingDelayReplicationTag)
		.AddUObject(this, &APlayerBase::WeaponChangingDelayReplicationTagChanged);

	// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
	// For now assume possession = spawn/respawn.
	InitializeAttributes();

	AddStartupEffects();

	AddCharacterAbilities();

	PlayerController = Cast<AActionPlayerController>(GetController());
	if (PlayerController)
	{
		//PC->CreateHUD();
	}

	if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
	{
		// Set Health/Mana/Stamina to their max. This is only necessary for *Respawn*.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
		SetShield(GetMaxShield());
	}

	// Remove Dead tag
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));

	//InitializeFloatingStatusBar();

	// If player is host on listen server, the floating status bar would have been created for them from BeginPlay before player possession, hide it
	/*if (IsLocallyControlled() && IsPlayerControlled() && UIFloatingStatusBarComponent && UIFloatingStatusBar)
	{
		UIFloatingStatusBarComponent->SetVisibility(false, true);
	}*/

	//SetupStartupPerspective();
}

void APlayerBase::FinishDying()
{
	if (!HasAuthority())
	{
		return;
	}

	RemoveAllWeaponsFromInventory();

	AbilitySystemComponent->RegisterGameplayTagEvent(WeaponChangingDelayReplicationTag).Remove(WeaponChangingDelayReplicationTagChangedDelegateHandle);

	AActionWorldRPGGameModeBase* GM = Cast<AActionWorldRPGGameModeBase>(GetWorld()->GetAuthGameMode());

	if (GM)
	{
		GM->PlayerDied(GetController());
	}

	RemoveCharacterAbilities();

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(DeathEffect->GetDefaultObject()), 1.0f, AbilitySystemComponent->MakeEffectContext());
	}

	OnCharacterDied.Broadcast(this);

	Super::FinishDying();
}

bool APlayerBase::IsInFirstPersonPerspective() const
{
	return bIsFirstPersonPerspective;
}

USkeletalMeshComponent* APlayerBase::GetThirdPersonMesh() const
{
	return GetMesh();
}

AWeaponBase* APlayerBase::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool APlayerBase::AddWeaponToInventory(AWeaponBase* NewWeapon, bool bEquipWeapon)
{
	if (DoesWeaponExistInInventory(NewWeapon))
	{
		USoundCue* PickupSound = NewWeapon->GetPickupSound();

		if (PickupSound && IsLocallyControlled())
		{
			UGameplayStatics::SpawnSoundAttached(PickupSound, GetRootComponent());
		}

		if (GetLocalRole() < ROLE_Authority)
		{
			return false;
		}

		// Create a dynamic instant Gameplay Effect to give the primary and secondary ammo
		UGameplayEffect* GEAmmo = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Ammo")));
		GEAmmo->DurationPolicy = EGameplayEffectDurationType::Instant;

		if (NewWeapon->PrimaryAmmoType != WeaponAmmoTypeNoneTag)
		{
			int32 Idx = GEAmmo->Modifiers.Num();
			GEAmmo->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoPrimaryAmmo = GEAmmo->Modifiers[Idx];
			InfoPrimaryAmmo.ModifierMagnitude = FScalableFloat(NewWeapon->GetPrimaryClipAmmo());
			InfoPrimaryAmmo.ModifierOp = EGameplayModOp::Additive;
			InfoPrimaryAmmo.Attribute = UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(NewWeapon->PrimaryAmmoType);
		}

		if (NewWeapon->SecondaryAmmoType != WeaponAmmoTypeNoneTag)
		{
			int32 Idx = GEAmmo->Modifiers.Num();
			GEAmmo->Modifiers.SetNum(Idx + 1);

			FGameplayModifierInfo& InfoSecondaryAmmo = GEAmmo->Modifiers[Idx];
			InfoSecondaryAmmo.ModifierMagnitude = FScalableFloat(NewWeapon->GetSecondaryClipAmmo());
			InfoSecondaryAmmo.ModifierOp = EGameplayModOp::Additive;
			InfoSecondaryAmmo.Attribute = UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(NewWeapon->SecondaryAmmoType);
		}

		if (GEAmmo->Modifiers.Num() > 0)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GEAmmo, 1.0f, AbilitySystemComponent->MakeEffectContext());
		}

		NewWeapon->Destroy();

		return false;
	}

	if (GetLocalRole() < ROLE_Authority)
	{
		return false;
	}

	Inventory.Weapons.Add(NewWeapon);
	NewWeapon->SetOwningCharacter(this);
	NewWeapon->AddAbilities();

	if (bEquipWeapon)
	{
		EquipWeapon(NewWeapon);
		ClientSyncCurrentWeapon(CurrentWeapon);
	}

	return true;
}

bool APlayerBase::RemoveWeaponFromInventory(AWeaponBase* WeaponToRemove)
{
	if (DoesWeaponExistInInventory(WeaponToRemove))
	{
		if (WeaponToRemove == CurrentWeapon)
		{
			UnEquipCurrentWeapon();
		}

		Inventory.Weapons.Remove(WeaponToRemove);
		WeaponToRemove->RemoveAbilities();
		WeaponToRemove->SetOwningCharacter(nullptr);
		WeaponToRemove->ResetWeapon();

		// Add parameter to drop weapon?

		return true;
	}

	return false;
}

void APlayerBase::RemoveAllWeaponsFromInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	UnEquipCurrentWeapon();

	float radius = 50.0f;
	float NumWeapons = Inventory.Weapons.Num();

	for (int32 i = Inventory.Weapons.Num() - 1; i >= 0; i--)
	{
		AWeaponBase* Weapon = Inventory.Weapons[i];
		RemoveWeaponFromInventory(Weapon);

		// Set the weapon up as a pickup

		float OffsetX = radius * FMath::Cos((i / NumWeapons) * 2.0f * PI);
		float OffsetY = radius * FMath::Sin((i / NumWeapons) * 2.0f * PI);
		Weapon->OnDropped(GetActorLocation() + FVector(OffsetX, OffsetY, 0.0f));
	}
}

void APlayerBase::EquipWeapon(AWeaponBase* NewWeapon)
{
	if (GetLocalRole() < ROLE_Authority)
	{
		ServerEquipWeapon(NewWeapon);
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
		bChangedWeaponLocally = true;
	}
	else
	{
		SetCurrentWeapon(NewWeapon, CurrentWeapon);
	}
}

void APlayerBase::ServerEquipWeapon_Implementation(AWeaponBase* NewWeapon)
{
	EquipWeapon(NewWeapon);
}

bool APlayerBase::ServerEquipWeapon_Validate(AWeaponBase* NewWeapon)
{
	return true;
}

void APlayerBase::NextWeapon()
{
	if (Inventory.Weapons.Num() < 2)
	{
		return;
	}

	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);
	UnEquipCurrentWeapon();

	if (CurrentWeaponIndex == INDEX_NONE)
	{
		EquipWeapon(Inventory.Weapons[0]);
	}
	else
	{
		EquipWeapon(Inventory.Weapons[(CurrentWeaponIndex + 1) % Inventory.Weapons.Num()]);
	}
}

void APlayerBase::PreviousWeapon()
{
	if (Inventory.Weapons.Num() < 2)
	{
		return;
	}

	int32 CurrentWeaponIndex = Inventory.Weapons.Find(CurrentWeapon);

	UnEquipCurrentWeapon();

	if (CurrentWeaponIndex == INDEX_NONE)
	{
		EquipWeapon(Inventory.Weapons[0]);
	}
	else
	{
		int32 IndexOfPrevWeapon = FMath::Abs(CurrentWeaponIndex - 1 + Inventory.Weapons.Num()) % Inventory.Weapons.Num();
		EquipWeapon(Inventory.Weapons[IndexOfPrevWeapon]);
	}
}

FName APlayerBase::GetWeaponAttachPoint()
{
	return WeaponAttachPoint;
}

int32 APlayerBase::GetPrimaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetPrimaryClipAmmo();
	}

	return 0;
}

int32 APlayerBase::GetMaxPrimaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetMaxPrimaryClipAmmo();
	}

	return 0;
}

int32 APlayerBase::GetPrimaryReserveAmmo() const
{
	if (CurrentWeapon && AmmoAttributeSet)
	{
		FGameplayAttribute Attribute = AmmoAttributeSet->GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType);
		if (Attribute.IsValid())
		{
			return AbilitySystemComponent->GetNumericAttribute(Attribute);
		}
	}

	return 0;
}

int32 APlayerBase::GetSecondaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetSecondaryClipAmmo();
	}

	return 0;
}

int32 APlayerBase::GetMaxSecondaryClipAmmo() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon->GetMaxSecondaryClipAmmo();
	}

	return 0;
}

int32 APlayerBase::GetSecondaryReserveAmmo() const
{
	if (CurrentWeapon)
	{
		FGameplayAttribute Attribute = AmmoAttributeSet->GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType);
		if (Attribute.IsValid())
		{
			return AbilitySystemComponent->GetNumericAttribute(Attribute);
		}
	}

	return 0;
}

int32 APlayerBase::GetNumWeapons() const
{
	return Inventory.Weapons.Num();
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	//InitializeFloatingStatusBar();

	// CurrentWeapon is replicated only to Simulated clients so sync the current weapon manually
	if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		ServerSyncCurrentWeapon();
	}
}

void APlayerBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clear CurrentWeaponTag on the ASC. This happens naturally in UnEquipCurrentWeapon() but
	// that is only called on the server from hero death (the OnRep_CurrentWeapon() would have
	// handled it on the client but that is never called due to the hero being marked pending
	// destroy). This makes sure the client has it cleared.
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
	}

	Super::EndPlay(EndPlayReason);
}

void APlayerBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//StartingThirdPersonCameraBoomArmLength = ThirdPersonCameraBoom->TargetArmLength;
	//StartingThirdPersonCameraBoomLocation = ThirdPersonCameraBoom->GetRelativeLocation();
	//StartingThirdPersonMeshLocation = GetMesh()->GetRelativeLocation();

	GetWorldTimerManager().SetTimerForNextTick(this, &APlayerBase::SpawnDefaultInventory);
}

void APlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		SetHUDCrosshairs(DeltaTime);
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}
}

void APlayerBase::HandleWeaponPrimaryActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::PrimaryFire);

	if (CurrentWeapon)
	{
		CrosshairShootingFactor = .5f;
	}
}

void APlayerBase::HandleWeaponPrimaryActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::PrimaryFire);
}

void APlayerBase::HandleWeaponSecondaryActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::SecondaryFire);
	bIsAiming = true;
	bUseControllerRotationYaw = true;
}

void APlayerBase::HandleWeaponSecondaryActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::SecondaryFire);
	bIsAiming = false;
	bUseControllerRotationYaw = false;
}

void APlayerBase::HandleWeaponAlternateActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::AlternateFire);
}

void APlayerBase::HandleWeaponAlternateActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::AlternateFire);
}

void APlayerBase::HandleReloadActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Reload);
}

void APlayerBase::HandleReloadActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::Reload);
}

void APlayerBase::HandleNextWeaponActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::NextWeapon);
}

void APlayerBase::HandleNextWeaponActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::NextWeapon);
}

void APlayerBase::HandlePreviousWeaponActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::PrevWeapon);
}

void APlayerBase::HandlePreviousWeaponActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::PrevWeapon);
}

void APlayerBase::CallGenericConfirm()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Confirm);
}

void APlayerBase::CallGenericCancel()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Cancel);
}

void APlayerBase::SendLocalInputToASC(bool bIsPressed, const EActionAbilityInputID AbilityInputID)
{
	if (!IsValid(AbilitySystemComponent))
	{
		return;
	}

	if (bIsPressed)
	{
		//UKismetSystemLibrary::PrintString(this, "Pressing Input");
		AbilitySystemComponent->AbilityLocalInputPressed(static_cast<int32>(AbilityInputID));
	}
	else
	{
		//UKismetSystemLibrary::PrintString(this, "Releasing Input");
		AbilitySystemComponent->AbilityLocalInputReleased(static_cast<int32>(AbilityInputID));
	}
}

void APlayerBase::TogglePerspective()
{
	// If knocked down, always be in 3rd person
	if (IsValid(AbilitySystemComponent))
	{
		return;
	}

	bIsFirstPersonPerspective = !bIsFirstPersonPerspective;
	SetPerspective(bIsFirstPersonPerspective);
}

void APlayerBase::SetPerspective(bool Is1PPerspective)
{// If knocked down, always be in 3rd person
	if (IsValid(AbilitySystemComponent))
	{
		return;
	}

	// Only change perspective for the locally controlled player. Simulated proxies should stay in third person.
	// To swap cameras, deactivate current camera (defaults to ThirdPersonCamera), activate desired camera, and call PlayerController->SetViewTarget() on self
	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalPlayerController())
	{
		//TODO
	}
}

void APlayerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init ASC Actor Info for clients. Server will init its ASC when it possesses a new Actor.
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
	//BindASCInput();

	AbilitySystemComponent->AbilityFailedCallbacks.AddUObject(this, &APlayerBase::OnAbilityActivationFailed);

	// If we handle players disconnecting and rejoining in the future, we'll have to change this so that posession from rejoining doesn't reset attributes.
	// For now assume possession = spawn/respawn.
	InitializeAttributes();

	AActionPlayerController* PC = Cast<AActionPlayerController>(GetController());
	if (PC)
	{
		//PC->CreateHUD();
	}

	if (CurrentWeapon)
	{
		// If current weapon repped before PlayerState, set tag on ASC
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
		// Update owning character and ASC just in case it repped before PlayerState
		CurrentWeapon->SetOwningCharacter(this);

		if (!PrimaryReserveAmmoChangedDelegateHandle.IsValid())
		{
			PrimaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType)).AddUObject(this, &APlayerBase::CurrentWeaponPrimaryReserveAmmoChanged);
		}
		if (!SecondaryReserveAmmoChangedDelegateHandle.IsValid())
		{
			SecondaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType)).AddUObject(this, &APlayerBase::CurrentWeaponSecondaryReserveAmmoChanged);
		}
	}

	if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
	{
		// Set Health/Mana/Stamina/Shield to their max. This is only for *Respawn*. It will be set (replicated) by the
		// Server, but we call it here just to be a little more responsive.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
		SetShield(GetMaxShield());
	}

	// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
	//InitializeFloatingStatusBar();
}

void APlayerBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	SetupStartupPerspective();
}

void APlayerBase::SpawnDefaultInventory()
{
	if (GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	int32 NumWeaponClasses = DefaultInventoryWeaponClasses.Num();
	for (int32 i = 0; i < NumWeaponClasses; i++)
	{
		if (!DefaultInventoryWeaponClasses[i])
		{
			// An empty item was added to the Array in blueprint
			continue;
		}

		AWeaponBase* NewWeapon = GetWorld()->SpawnActorDeferred<AWeaponBase>(DefaultInventoryWeaponClasses[i],
			FTransform::Identity, this, this, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		NewWeapon->bSpawnWithCollision = false;
		NewWeapon->FinishSpawning(FTransform::Identity);

		bool bEquipFirstWeapon = i == 0;
		AddWeaponToInventory(NewWeapon, bEquipFirstWeapon);
	}
}

void APlayerBase::SetupStartupPerspective()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && PC->IsLocalController())
	{
		SetPerspective(bIsFirstPersonPerspective);
	}
}

bool APlayerBase::DoesWeaponExistInInventory(AWeaponBase* InWeapon)
{
	for (AWeaponBase* Weapon : Inventory.Weapons)
	{
		if (Weapon && InWeapon && Weapon->GetClass() == InWeapon->GetClass())
		{
			return true;
		}
	}

	return false;
}

void APlayerBase::SetCurrentWeapon(AWeaponBase* NewWeapon, AWeaponBase* LastWeapon)
{
	if (NewWeapon == LastWeapon)
	{
		return;
	}

	// Cancel active weapon abilities
	if (AbilitySystemComponent)
	{
		FGameplayTagContainer AbilityTagsToCancel = FGameplayTagContainer(WeaponAbilityTag);
		AbilitySystemComponent->CancelAbilities(&AbilityTagsToCancel);
	}

	UnEquipWeapon(LastWeapon);

	if (NewWeapon)
	{
		if (AbilitySystemComponent)
		{
			// Clear out potential NoWeaponTag
			AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		}

		// Weapons coming from OnRep_CurrentWeapon won't have the owner set
		CurrentWeapon = NewWeapon;
		CurrentWeapon->SetOwningCharacter(this);
		CurrentWeapon->Equip();
		CurrentWeaponTag = CurrentWeapon->WeaponTag;

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
		}

		AActionPlayerController* PC = GetController<AActionPlayerController>();
		if (PC && PC->IsLocalController())
		{
			PC->SetEquippedWeaponPrimaryIconFromSprite(CurrentWeapon->PrimaryIcon);
			PC->SetEquippedWeaponStatusText(CurrentWeapon->StatusText);
			PC->SetPrimaryClipAmmo(CurrentWeapon->GetPrimaryClipAmmo());
			PC->SetPrimaryReserveAmmo(GetPrimaryReserveAmmo());
			//PC->SetHUDReticle(CurrentWeapon->GetPrimaryHUDReticleClass());
		}

		NewWeapon->OnPrimaryClipAmmoChanged.AddDynamic(this, &APlayerBase::CurrentWeaponPrimaryClipAmmoChanged);
		NewWeapon->OnSecondaryClipAmmoChanged.AddDynamic(this, &APlayerBase::CurrentWeaponSecondaryClipAmmoChanged);

		if (AbilitySystemComponent)
		{
			PrimaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(CurrentWeapon->PrimaryAmmoType)).AddUObject(this, &APlayerBase::CurrentWeaponPrimaryReserveAmmoChanged);
			SecondaryReserveAmmoChangedDelegateHandle = AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(CurrentWeapon->SecondaryAmmoType)).AddUObject(this, &APlayerBase::CurrentWeaponSecondaryReserveAmmoChanged);
		}

		UAnimMontage* Equip3PMontage = CurrentWeapon->GetEquip3PMontage();
		if (Equip3PMontage && GetThirdPersonMesh())
		{
			GetThirdPersonMesh()->GetAnimInstance()->Montage_Play(Equip3PMontage);
		}
	}
	else
	{
		// This will clear HUD, tags etc
		UnEquipCurrentWeapon();
	}
}

void APlayerBase::UnEquipWeapon(AWeaponBase* WeaponToUnEquip)
{//TODO this will run into issues when calling UnEquipWeapon explicitly and the WeaponToUnEquip == CurrentWeapon

	if (WeaponToUnEquip)
	{
		WeaponToUnEquip->OnPrimaryClipAmmoChanged.RemoveDynamic(this, &APlayerBase::CurrentWeaponPrimaryClipAmmoChanged);
		WeaponToUnEquip->OnSecondaryClipAmmoChanged.RemoveDynamic(this, &APlayerBase::CurrentWeaponSecondaryClipAmmoChanged);

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(WeaponToUnEquip->PrimaryAmmoType)).Remove(PrimaryReserveAmmoChangedDelegateHandle);
			AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetReserveAmmoAttributeFromTag(WeaponToUnEquip->SecondaryAmmoType)).Remove(SecondaryReserveAmmoChangedDelegateHandle);
		}

		WeaponToUnEquip->UnEquip();
	}
}

void APlayerBase::UnEquipCurrentWeapon()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->RemoveLooseGameplayTag(CurrentWeaponTag);
		CurrentWeaponTag = NoWeaponTag;
		AbilitySystemComponent->AddLooseGameplayTag(CurrentWeaponTag);
	}

	UnEquipWeapon(CurrentWeapon);
	CurrentWeapon = nullptr;

	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->SetEquippedWeaponPrimaryIconFromSprite(nullptr);
		PC->SetEquippedWeaponStatusText(FText());
		PC->SetPrimaryClipAmmo(0);
		PC->SetPrimaryReserveAmmo(0);
		//PC->SetHUDReticle(nullptr);
	}
}

void APlayerBase::CurrentWeaponPrimaryClipAmmoChanged(int32 OldPrimaryClipAmmo, int32 NewPrimaryClipAmmo)
{
	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->SetPrimaryClipAmmo(NewPrimaryClipAmmo);
	}
}

void APlayerBase::CurrentWeaponSecondaryClipAmmoChanged(int32 OldSecondaryClipAmmo, int32 NewSecondaryClipAmmo)
{
	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->SetSecondaryClipAmmo(NewSecondaryClipAmmo);
	}
}

void APlayerBase::CurrentWeaponPrimaryReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->SetPrimaryReserveAmmo(Data.NewValue);
	}
}

void APlayerBase::CurrentWeaponSecondaryReserveAmmoChanged(const FOnAttributeChangeData& Data)
{
	AActionPlayerController* PC = GetController<AActionPlayerController>();
	if (PC && PC->IsLocalController())
	{
		PC->SetSecondaryReserveAmmo(Data.NewValue);
	}
}

void APlayerBase::WeaponChangingDelayReplicationTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	if (CallbackTag == WeaponChangingDelayReplicationTag)
	{
		if (NewCount < 1)
		{
			// We only replicate the current weapon to simulated proxies so manually sync it when the weapon changing delay replication
			// tag is removed. We keep the weapon changing tag on for ~1s after the equip montage to allow for activating changing weapon
			// again without the server trying to clobber the next locally predicted weapon.
			ClientSyncCurrentWeapon(CurrentWeapon);
		}
	}
}

void APlayerBase::OnRep_CurrentWeapon(AWeaponBase* LastWeapon)
{
	bChangedWeaponLocally = false;
	SetCurrentWeapon(CurrentWeapon, LastWeapon);
}

void APlayerBase::OnRep_Inventory()
{
	if (GetLocalRole() == ROLE_AutonomousProxy && Inventory.Weapons.Num() > 0 && !CurrentWeapon)
	{
		// Since we don't replicate the CurrentWeapon to the owning client, this is a way to ask the Server to sync
		// the CurrentWeapon after it's been spawned via replication from the Server.
		// The weapon spawning is replicated but the variable CurrentWeapon is not on the owning client.
		ServerSyncCurrentWeapon();
	}
}

void APlayerBase::OnAbilityActivationFailed(const UGameplayAbility* FailedAbility, const FGameplayTagContainer& FailTags)
{
	if (FailedAbility && FailedAbility->AbilityTags.HasTagExact(FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.IsChanging"))))
	{
		if (bChangedWeaponLocally)
		{
			// Ask the Server to resync the CurrentWeapon that we predictively changed
			UE_LOG(LogTemp, Warning, TEXT("Weapon Changing ability activation failed. Syncing CurrentWeapon."));

			ServerSyncCurrentWeapon();
		}
	}
}

void APlayerBase::ServerSyncCurrentWeapon_Implementation()
{
	ClientSyncCurrentWeapon(CurrentWeapon);
}

bool APlayerBase::ServerSyncCurrentWeapon_Validate()
{
	return true;
}

void APlayerBase::ClientSyncCurrentWeapon_Implementation(AWeaponBase* InWeapon)
{
	AWeaponBase* LastWeapon = CurrentWeapon;
	CurrentWeapon = InWeapon;
	OnRep_CurrentWeapon(LastWeapon);
}

bool APlayerBase::ClientSyncCurrentWeapon_Validate(AWeaponBase* InWeapon)
{
	return true;
}

void APlayerBase::SetHUDCrosshairs(float DeltaTime)
{
	if (PlayerController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			TEXT("PlayerBase Cant Find Controller"));
		return;
	}

	//만약에 HUD가 존재하면 그냥 뱉고 아니면 캐스팅
	PlayerHUD = PlayerHUD == nullptr ? Cast<APlayerHUD>(PlayerController->GetHUD()) : PlayerHUD;

	if (PlayerHUD)
	{
		if (CurrentWeapon)
		{
			HUDPackage.CrosshairsCenter = CurrentWeapon->CrosshairsCenter;
			HUDPackage.CrosshairsLeft = CurrentWeapon->CrosshairsLeft;
			HUDPackage.CrosshairsRight = CurrentWeapon->CrosshairsRight;
			HUDPackage.CrosshairsTop = CurrentWeapon->CrosshairsTop;
			HUDPackage.CrosshairsBottom = CurrentWeapon->CrosshairsBottom;
		}
		else
		{
			HUDPackage.CrosshairsCenter = nullptr;
			HUDPackage.CrosshairsLeft = nullptr;
			HUDPackage.CrosshairsRight = nullptr;
			HUDPackage.CrosshairsTop = nullptr;
			HUDPackage.CrosshairsBottom = nullptr;
		}
		//Calculate Crosshair Spread
		//Player MaxSpeed = 500.f
		FVector2D WalkSpeedRange(0.f, 500.f);
		FVector2D VelocityMultiplierRange(0.f, 1.f);
		FVector Velocity = GetVelocity();
		Velocity.Z = 0.f;
		CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(
			WalkSpeedRange, VelocityMultiplierRange, Velocity.Size());

		if (GetCharacterMovement()->IsFalling())
		{
			CrosshairInAirFactor = FMath::FInterpTo(
				CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
		}
		else
		{
			CrosshairInAirFactor = FMath::FInterpTo(
				CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
		}

		if (bIsAiming)
		{
			CrosshairAimFactor = FMath::FInterpTo(
				CrosshairAimFactor, .58f, DeltaTime, 30.f);
		}
		else
		{
			CrosshairAimFactor = FMath::FInterpTo(
				CrosshairAimFactor, 0.f, DeltaTime, 30.f);
		}

		CrosshairShootingFactor = FMath::FInterpTo(
			CrosshairShootingFactor, 0.f, DeltaTime, 40.f);

		HUDPackage.CrosshairSpread =
			0.5f +
			CrosshairVelocityFactor +
			CrosshairInAirFactor -
			CrosshairAimFactor +
			CrosshairShootingFactor;

		PlayerHUD->SetHUDPackage(HUDPackage);
	}

}

FVector APlayerBase::GetHitTarget() const
{
	return HitTarget;
}

void APlayerBase::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		DrawDebugSphere(GetWorld(), Start, 16.f, 12, FColor::Red, false);

		FVector End = Start + CrosshairWorldDirection * 80000.f;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractCrosshairInterface>())
		{
			HUDPackage.CrosshairColor = FLinearColor::Red;
		}
		else
		{
			HUDPackage.CrosshairColor = FLinearColor::White;
		}
	}
}

void APlayerBase::InterpFOV(float DeltaTime)
{
	if (CurrentWeapon == nullptr) return;

	float CurrentFOV = CameraComp->FieldOfView;
	if (bIsAiming)
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, CurrentWeapon->ZoomFOV,
			DeltaTime, CurrentWeapon->ZoomDuration);
	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, Default3PFOV,
			DeltaTime, CurrentWeapon->ZoomDuration / 2.f);
	}

	if (CameraComp)
	{
		CameraComp->SetFieldOfView(CurrentFOV);
	}
}

void APlayerBase::SetHitTarget(FVector hitTarget)
{
	HitTarget = hitTarget;
}
