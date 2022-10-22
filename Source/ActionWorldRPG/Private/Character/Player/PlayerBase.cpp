// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/PlayerBase.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

//�����Ƽ
#include "AbilitySystem/Attributes/AmmoAttributeSet.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//ī�޶�
#include "Engine/Classes/GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

//ĳ���� �⺻
#include "Engine/Classes/Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ActionWorldRPG/ActionWorldRPGGameModeBase.h"
#include "Character/Player/ActionPlayerController.h"

//����
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

//����
#include "Weapon/WeaponBase.h"

//�Է�
#include "InputAction.h"
#include "EnhancedInputComponent.h"

//UI
#include "HUD/InteractionHUD.h"
#include "HUD/QuestWidget.h"
#include "MotionWarpingComponent.h"
#include "Character/Player/CustomCharacterMovementComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

//Quest
#include "Component/Quest/QuestSystemComponent.h"

APlayerBase::APlayerBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))

{
	PrimaryActorTick.bCanEverTick = true;

	bIsFirstPersonPerspective = false;
	Default3PFOV = 80.0f;
	NoWeaponTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Equipped.None"));
	WeaponChangingDelayReplicationTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon.IsChangingDelayReplication"));
	WeaponAmmoTypeNoneTag = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));
	WeaponAbilityTag = FGameplayTag::RequestGameplayTag(FName("Ability.Weapon"));
	CurrentWeaponTag = NoWeaponTag;

	//�������� ������Ʈ ����
	CameraSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	CameraSpringArmComp->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 108.f),
		FRotator(0.f, 0.f, 90.f));
	CameraSpringArmComp->bUsePawnControlRotation = true;
	CameraSpringArmComp->SetupAttachment(GetMesh());

	//ī�޶� ������Ʈ ����
	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	CameraComp->SetupAttachment(CameraSpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	MotionWarpingComp = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarpingComponent"));

	//ĸ��������Ʈ
	GetCapsuleComponent()->InitCapsuleSize(94.f, 42.f);
	//AreaClass -> NavArea_Obstacle

	//�Ž�
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -100.f),
		FRotator(0.f, 0.f, 270.f));

	//ĳ���� �����Ʈ ����
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

	MovementComponent = Cast<UCustomCharacterMovementComponent>(GetCharacterMovement());

	//SpawnCollisionHandlingMethod Always Spawn, Ignore Collisions
}

void APlayerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
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
		if (EmoteAction)
		{
			EnhancedInputComponent->BindAction(EmoteAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleEmoteActionPressed);
			EnhancedInputComponent->BindAction(EmoteAction, ETriggerEvent::Completed, this, &APlayerBase::HandleEmoteActionReleased);
		}
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &APlayerBase::HandleInteractActionPressed);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &APlayerBase::HandleInteractActionReleased);
		}
		if (SlidingAction)
		{
			EnhancedInputComponent->BindAction(SlidingAction, ETriggerEvent::Triggered, this, &ThisClass::HandleSlidingActionPressed);
			EnhancedInputComponent->BindAction(SlidingAction, ETriggerEvent::Completed, this, &ThisClass::HandleSlidingActionReleased);
		}
	}
}

void APlayerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	PlayerController = Cast<AActionPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->CreateInteractionHUD();
	}

	// Bind player input to the AbilitySystemComponent. Also called in SetupPlayerInputComponent because of a potential race condition.
	//BindASCInput();

	//�������� ȣ��˴ϴ�.
	if (AbilitySystemComponent->GetTagCount(DeadTag) > 0)
	{
		// Set Health/Mana/Stamina/Shield to their max. This is only for *Respawn*. It will be set (replicated) by the
		// Server, but we call it here just to be a little more responsive.
		SetHealth(GetMaxHealth());
		SetMana(GetMaxMana());
		SetStamina(GetMaxStamina());
		SetShield(GetMaxShield());
	}

	//// Simulated on proxies don't have their PlayerStates yet when BeginPlay is called so we call it again here
	//InitializeFloatingStatusBar();
}

void APlayerBase::OnRep_Controller()
{
	Super::OnRep_Controller();

	SetupStartupPerspective();
}

void APlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	PlayerController = Cast<AActionPlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->CreateInteractionHUD();
	}

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

	if (AbilitySystemComponent)
	{
		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);
	}

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

bool APlayerBase::AddWeaponToInventory(AWeaponBase* NewWeapon, bool bEquipWeapon)
{
	if (NewWeapon)
	{
		USoundCue* PickupSound = NewWeapon->GetPickupSound();

		if (PickupSound && IsLocallyControlled())
		{
			UGameplayStatics::SpawnSoundAttached(PickupSound, GetRootComponent());
		}

		NewWeapon->SetOwningCharacter(this);
		NewWeapon->AddAbilities();

		return true;
	}

	return false;
}

bool APlayerBase::RemoveWeaponFromInventory(AWeaponBase* WeaponToRemove)
{
	if (WeaponToRemove)
	{
		WeaponToRemove->RemoveAbilities();
		WeaponToRemove->SetOwningCharacter(nullptr);

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

}

FName APlayerBase::GetWeaponAttachPoint()
{
	return WeaponAttachPoint;
}

void APlayerBase::BeginPlay()
{
	Super::BeginPlay();

	// Only needed for Heroes placed in world and when the player is the Server.
	// On respawn, they are set up in PossessedBy.
	// When the player a client, the floating status bars are all set up in OnRep_PlayerState.
	//InitializeFloatingStatusBar();

	//����Ʈ ������Ʈ�� ã�������� �������ݴϴ�.
	FindQuestComponent();
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

	if (true)
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
	bIsAiming = true;
	bUseControllerRotationYaw = true;
}

void APlayerBase::HandleWeaponAlternateActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::AlternateFire);
	bIsAiming = false;
	bUseControllerRotationYaw = false;
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

void APlayerBase::HandleEmoteActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Emote);
}

void APlayerBase::HandleEmoteActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::Emote);
}

void APlayerBase::HandleInteractActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Interact);
}

void APlayerBase::HandleInteractActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::Interact);
}

void APlayerBase::HandleSlidingActionPressed()
{
	SendLocalInputToASC(true, EActionAbilityInputID::Sliding);
}

void APlayerBase::HandleSlidingActionReleased()
{
	SendLocalInputToASC(false, EActionAbilityInputID::Sliding);
}

void APlayerBase::Climb()
{
	MovementComponent->TryClimbing();
}

void APlayerBase::CancelClimb()
{
	MovementComponent->CancelClimbing();
}

void APlayerBase::ClimbMoveForward(float Value)
{
	FVector Direction;
	Direction = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), -GetActorRightVector());

	//ĸ���� ��ġ���˻��մϴ�? �� ��ġ�� ���� �� �ִ� ���� �ִٸ�? �����̽��ٸ� �������� �̵��մϴ�?
	//�뽬�� ���
	//AddMovementInput(Direction, Value);
}

void APlayerBase::ClimbMoveRight(float Value)
{
	FVector Direction;
	Direction = FVector::CrossProduct(MovementComponent->GetClimbSurfaceNormal(), GetActorUpVector());

	//�̵��� ���ִ� ������ ���ٸ� �������� �ʽ��ϴ�. ȿ�����ϱ�?
	FHitResult Result;
	TArray<AActor*> IgnoreActor;
	FVector Start = GetActorLocation() + GetActorUpVector() * 100 + Direction * Value * 35;
	FVector End = Start + GetActorForwardVector() * 50;
	bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10, TraceTypeQuery1,
		false, IgnoreActor, EDrawDebugTrace::ForOneFrame, Result, true);

	//¤���� �ִ� ������ �ִ��� Ȯ���մϴ�.
	if (bIsHit)
	{
		FHitResult NewResult;
		bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Result.ImpactPoint + FVector(0, 0, 20), Result.ImpactPoint, 10, TraceTypeQuery1,
			false, IgnoreActor, EDrawDebugTrace::ForOneFrame, NewResult, true);

		//¤���� �ִ� ������ �ִٸ� �̵��մϴ�.
		if (bIsNewHit && !MovementComponent->GetIsJumpClimb())
		{
			AddMovementInput(Direction, Value);
		}
	}
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

void APlayerBase::SetupStartupPerspective()
{
	APlayerController* PC = Cast<APlayerController>(GetController());

	if (PC && PC->IsLocalController())
	{
		SetPerspective(bIsFirstPersonPerspective);
	}
}

bool APlayerBase::CalculateWallOverPoint(FVector AnimRootStartPoint, int32 WarpStartForwardMul, int32 WarpLandForwardMul, int32 WarpEndForwardMul)
{
	bool bWarp = false;

	//�Փ��� 1���� ���濡 ��ü�� �ִ��� Ž���մϴ�
	TArray<AActor*> ActorToIgnore;
	FHitResult Result;
	UKismetSystemLibrary::SphereTraceSingle(
		this,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * 100,
		10,
		TraceTypeQuery1,
		false,
		ActorToIgnore,
		EDrawDebugTrace::ForDuration,
		Result,
		true);

	//���� �� �ִ� ��������
	if (Result.GetActor() != nullptr)
	{
		FVector Normal = Result.Normal;

		WarpRotation = UKismetMathLibrary::Conv_VectorToRotator(Normal * -1);

		//���˵��������� �Փ������� 25�� �����Ѵ� ��2.5������ ������ �˻��Ѵ�
		bool bOnce = false;
		for (int i = 0; i < 10; ++i)
		{
			//���������� �Ʒ��� �˻��ϸ鼭 �浹������ ã�´� 1.5���� ���������� �˻��Ѵ�
			//�ְ���� ������ Ư���������ִٸ� ���� �� ����
			TArray<AActor*> NewActorToIgnore;
			FHitResult NewResult;
			UKismetSystemLibrary::SphereTraceSingle(
				this,
				Result.ImpactPoint + i * -25 * Normal + FVector(0, 0, 150),
				Result.ImpactPoint + i * -25 * Normal,
				10,
				TraceTypeQuery1,
				false,
				NewActorToIgnore,
				EDrawDebugTrace::ForDuration,
				NewResult,
				true);

			if (NewResult.GetActor() != nullptr)
			{
				if (!bOnce)
				{
					bOnce = true;
					//�ִϸ��̼��� Root�� �Ǵ� ���� //30
					WarpStartPoint = NewResult.ImpactPoint + Normal * -WarpStartForwardMul + AnimRootStartPoint;
				}

				//�浹������ ���� �� ����
				WarpEdgePoint = NewResult.ImpactPoint;
			}

			//��������
			if (i == 9)
			{
				TArray<AActor*> LandActorToIgnore;
				FHitResult LandResult;
				//�ǳ��浹�������� �Ʒ��� 3���� �˻��Ѵ� //200
				UKismetSystemLibrary::SphereTraceSingle(
					this,
					WarpEdgePoint + Normal * -WarpLandForwardMul,
					WarpEdgePoint + Normal * -WarpLandForwardMul + FVector(0, 0, -300),
					10,
					TraceTypeQuery1,
					false,
					LandActorToIgnore,
					EDrawDebugTrace::ForDuration,
					LandResult,
					true);

				if (LandResult.GetActor() != nullptr)
				{
					WarpLandPoint = LandResult.ImpactPoint;
					bWarp = true;
				}
			}
		}

		//������������ �ణ�� //50
		WarpEndPoint = WarpLandPoint + Normal * -WarpEndForwardMul;
		return bWarp;
	}

	return bWarp;
}

void APlayerBase::SetHUDCrosshairs(float DeltaTime)
{
	if (PlayerController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
			TEXT("PlayerBase Cant Find Controller"));
		return;
	}

	//���࿡ HUD�� �����ϸ� �׳� ��� �ƴϸ� ĳ����
	PlayerHUD = PlayerHUD == nullptr ? Cast<APlayerHUD>(PlayerController->GetHUD()) : PlayerHUD;

	if (PlayerHUD)
	{
		if (true)
		{
			/*HUDPackage.CrosshairsCenter = CurrentWeapon->CrosshairsCenter;
			HUDPackage.CrosshairsLeft = CurrentWeapon->CrosshairsLeft;
			HUDPackage.CrosshairsRight = CurrentWeapon->CrosshairsRight;
			HUDPackage.CrosshairsTop = CurrentWeapon->CrosshairsTop;
			HUDPackage.CrosshairsBottom = CurrentWeapon->CrosshairsBottom;*/
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

		if (MovementComponent->IsFalling())
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

void APlayerBase::ShowInteractionPrompt(float InteractionDuration)
{
	if (PlayerController != nullptr)
	{
		UInteractionHUD* HUD = PlayerController->GetInteractionHUD();
		if (HUD)
		{
			HUD->ShowInteractionPrompt(InteractionDuration);
		}
	}
}

void APlayerBase::HideInteractionPrompt()
{
	if (PlayerController != nullptr)
	{
		UInteractionHUD* HUD = PlayerController->GetInteractionHUD();
		if (HUD)
		{
			HUD->HideInteractionPrompt();
		}
	}
}

void APlayerBase::StartInteractionTimer(float InteractionDuration)
{
	if (PlayerController != nullptr)
	{
		UInteractionHUD* HUD = PlayerController->GetInteractionHUD();
		if (HUD)
		{
			HUD->StartInteractionTimer(InteractionDuration);
		}
	}
}

void APlayerBase::StopInteractionTimer()
{
	if (PlayerController != nullptr)
	{
		UInteractionHUD* HUD = PlayerController->GetInteractionHUD();
		if (HUD)
		{
			HUD->StopInteractionTimer();
		}
	}
}

void APlayerBase::InterpFOV(float DeltaTime)
{
	/*if (CurrentWeapon == nullptr) return;

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
	}*/
}

void APlayerBase::SetHitTarget(FVector hitTarget)
{
	HitTarget = hitTarget;
}

void APlayerBase::FindQuestComponent()
{
	QuestComponent = FindComponentByClass<UQuestSystemComponent>();
	if (QuestComponent == nullptr)
	{
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("FindQuestComponent"));
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
	}
	else
	{
		//���� ���� �� �ִ�
		if (QuestWidgetClass != nullptr)
		{
			QuestWidget = CreateWidget<UQuestWidget>(PlayerController, QuestWidgetClass);
			//�⺻����
			QuestWidget->BindQuestDelegateFunction(this);
			UWidget* RootWidget = PlayerController->GetBlackOutWidget()->GetRootWidget();
			UCanvasPanelSlot* QuestSlot = Cast<UCanvasPanel>(RootWidget)->AddChildToCanvas(QuestWidget);
			//QuestWidget->AddToViewport();
			QuestSlot->SetAnchors(FAnchors(0, 0.5, 0, 0.5));
			QuestSlot->SetSize(FVector2D(500, 150));
			QuestSlot->SetAlignment(FVector2D(0, 0.5));
			QuestSlot->SetPosition(FVector2D(0, 0));
		}
	}
}

EPhysicalSurface APlayerBase::GetSurfaceType()
{
	FHitResult HitResult;
	const FVector Start{ GetActorLocation() };
	const FVector End{ Start + FVector(0.f, 0.f, -400.f) };
	FCollisionQueryParams QueryParams; // has Physical Material
	QueryParams.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility,
		QueryParams);

	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}