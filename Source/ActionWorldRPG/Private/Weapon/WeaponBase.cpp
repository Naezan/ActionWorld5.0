// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"

//캐릭터
#include "Character/Player/PlayerBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ActionGameplayAbility.h"

//네트워크
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionComponent"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Manually enable when in pickup mode
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	WeaponMesh3PickupRelativeLocation = FVector(0.0f, -25.0f, 0.0f);

	WeaponMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh3P->SetupAttachment(CollisionComp);

	WeaponMesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);

	// Set this actor to never tick
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 100.0f; // Set this to a value that's appropriate for your game
	bSpawnWithCollision = true;
	PrimaryClipAmmo = 0;
	MaxPrimaryClipAmmo = 0;
	SecondaryClipAmmo = 0;
	MaxSecondaryClipAmmo = 0;
	bInfiniteAmmo = false;
	PrimaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));
	SecondaryAmmoType = FGameplayTag::RequestGameplayTag(FName("Weapon.Ammo.None"));

	WeaponPrimaryInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Primary.Instant");
	WeaponSecondaryInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Secondary.Instant");
	WeaponAlternateInstantAbilityTag = FGameplayTag::RequestGameplayTag("Ability.Weapon.Alternate.Instant");
	WeaponIsFiringTag = FGameplayTag::RequestGameplayTag("Weapon.IsFiring");

	FireMode = FGameplayTag::RequestGameplayTag("Weapon.FireMode.None");
	StatusText = DefaultStatusText;

	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.KnockedDown"));
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	ResetWeapon();

	if (!OwningCharacter && bSpawnWithCollision)
	{
		// Spawned into the world without an owner, enable collision as we are in pickup mode
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}

	Super::BeginPlay();
}

UAbilitySystemComponent* AWeaponBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

USkeletalMeshComponent* AWeaponBase::GetWeaponMesh3P() const
{
	return WeaponMesh3P;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, OwningCharacter, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, PrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, MaxPrimaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, SecondaryClipAmmo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AWeaponBase, MaxSecondaryClipAmmo, COND_OwnerOnly);
}

void AWeaponBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);

	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeaponBase, PrimaryClipAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));
	DOREPLIFETIME_ACTIVE_OVERRIDE(AWeaponBase, SecondaryClipAmmo, (IsValid(AbilitySystemComponent) && !AbilitySystemComponent->HasMatchingGameplayTag(WeaponIsFiringTag)));

}

void AWeaponBase::SetOwningCharacter(APlayerBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		// Called when added to inventory
		AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		if (OwningCharacter->GetCurrentWeapon() != this)
		{
			WeaponMesh3P->CastShadow = false;
			WeaponMesh3P->SetVisibility(true, true);
			WeaponMesh3P->SetVisibility(false, true);
		}
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AWeaponBase::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	if (IsValid(this) && !OwningCharacter)
	{
		PickUpOnTouch(Cast<APlayerBase>(Other));
	}
}

void AWeaponBase::Equip()
{
	if (!OwningCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("%s %s OwningCharacter is nullptr"), *FString(__FUNCTION__), *GetName());
		return;
	}

	FName AttachPoint = OwningCharacter->GetWeaponAttachPoint();
	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(OwningCharacter->GetThirdPersonMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, AttachPoint);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PEquippedRelativeLocation);
		WeaponMesh3P->SetRelativeRotation(FRotator(0, 0, -90.0f));
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->bCastHiddenShadow = true;

		if (OwningCharacter->IsInFirstPersonPerspective())
		{
			WeaponMesh3P->SetVisibility(true, true); // Without this, the weapon's 3p shadow doesn't show
			WeaponMesh3P->SetVisibility(false, true);
		}
		else
		{
			WeaponMesh3P->SetVisibility(true, true);
		}
	}
}

void AWeaponBase::UnEquip()
{
	if (OwningCharacter == nullptr)
	{
		return;
	}

	// Necessary to detach so that when toggling perspective all meshes attached won't become visible.

	WeaponMesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	WeaponMesh3P->CastShadow = false;
	WeaponMesh3P->bCastHiddenShadow = false;
	WeaponMesh3P->SetVisibility(true, true); // Without this, the unequipped weapon's 3p shadow hangs around
	WeaponMesh3P->SetVisibility(false, true);
}

void AWeaponBase::AddAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UActionAbilitySystemComponent* ASC = Cast<UActionAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Weapon UActionAbilitySystemComponent is null"));
		return;
	}

	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (TSubclassOf<UActionGameplayAbility>& Ability : Abilities)
	{
		AbilitySpecHandles.Add(ASC->GiveAbility(
			FGameplayAbilitySpec(Ability, 1, INDEX_NONE, this)));
	}
}

void AWeaponBase::RemoveAbilities()
{
	if (!IsValid(OwningCharacter) || !OwningCharacter->GetAbilitySystemComponent())
	{
		return;
	}

	UActionAbilitySystemComponent* ASC = Cast<UActionAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());

	if (!ASC)
	{
		return;
	}

	// Remove abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	for (FGameplayAbilitySpecHandle& SpecHandle : AbilitySpecHandles)
	{
		ASC->ClearAbility(SpecHandle);
	}
}

int32 AWeaponBase::GetAbilityLevel(EActionAbilityInputID AbilityID)
{
	// All abilities for now are level 1
	return 1;
}

void AWeaponBase::ResetWeapon()
{
	FireMode = DefaultFireMode;
	StatusText = DefaultStatusText;
}

void AWeaponBase::OnDropped_Implementation(FVector NewLocation)
{
	SetOwningCharacter(nullptr);
	ResetWeapon();

	SetActorLocation(NewLocation);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (WeaponMesh3P)
	{
		WeaponMesh3P->AttachToComponent(CollisionComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
		WeaponMesh3P->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);
		WeaponMesh3P->CastShadow = true;
		WeaponMesh3P->SetVisibility(true, true);
	}
}

bool AWeaponBase::OnDropped_Validate(FVector NewLocation)
{
	return true;
}

int32 AWeaponBase::GetPrimaryClipAmmo() const
{
	return PrimaryClipAmmo;
}

int32 AWeaponBase::GetMaxPrimaryClipAmmo() const
{
	return MaxPrimaryClipAmmo;
}

int32 AWeaponBase::GetSecondaryClipAmmo() const
{
	return SecondaryClipAmmo;
}

int32 AWeaponBase::GetMaxSecondaryClipAmmo() const
{
	return MaxSecondaryClipAmmo;
}

void AWeaponBase::SetPrimaryClipAmmo(int32 NewPrimaryClipAmmo)
{
	int32 OldPrimaryClipAmmo = PrimaryClipAmmo;
	PrimaryClipAmmo = NewPrimaryClipAmmo;
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void AWeaponBase::SetMaxPrimaryClipAmmo(int32 NewMaxPrimaryClipAmmo)
{
	int32 OldMaxPrimaryClipAmmo = MaxPrimaryClipAmmo;
	MaxPrimaryClipAmmo = NewMaxPrimaryClipAmmo;
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void AWeaponBase::SetSecondaryClipAmmo(int32 NewSecondaryClipAmmo)
{
	int32 OldSecondaryClipAmmo = SecondaryClipAmmo;
	SecondaryClipAmmo = NewSecondaryClipAmmo;
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void AWeaponBase::SetMaxSecondaryClipAmmo(int32 NewMaxSecondaryClipAmmo)
{
	int32 OldMaxSecondaryClipAmmo = MaxSecondaryClipAmmo;
	MaxSecondaryClipAmmo = NewMaxSecondaryClipAmmo;
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}

bool AWeaponBase::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

UAnimMontage* AWeaponBase::GetEquip3PMontage() const
{
	return Equip3PMontage;
}

USoundCue* AWeaponBase::GetPickupSound() const
{
	return PickupSound;
}

FText AWeaponBase::GetDefaultStatusText() const
{
	return DefaultStatusText;
}

void AWeaponBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	/*if (LineTraceTargetActor)
	{
		LineTraceTargetActor->Destroy();
	}

	if (SphereTraceTargetActor)
	{
		SphereTraceTargetActor->Destroy();
	}*/

	Super::EndPlay(EndPlayReason);
}

void AWeaponBase::PickUpOnTouch(APlayerBase* InCharacter)
{
	if (!InCharacter || !InCharacter->IsAlive() || !InCharacter->GetAbilitySystemComponent() || InCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags))
	{
		return;
	}

	if (InCharacter->AddWeaponToInventory(this, true) && OwningCharacter->IsInFirstPersonPerspective())
	{
		WeaponMesh3P->CastShadow = false;
		WeaponMesh3P->SetVisibility(true, true);
		WeaponMesh3P->SetVisibility(false, true);
	}
}

void AWeaponBase::OnRep_PrimaryClipAmmo(int32 OldPrimaryClipAmmo)
{
	OnPrimaryClipAmmoChanged.Broadcast(OldPrimaryClipAmmo, PrimaryClipAmmo);
}

void AWeaponBase::OnRep_MaxPrimaryClipAmmo(int32 OldMaxPrimaryClipAmmo)
{
	OnMaxPrimaryClipAmmoChanged.Broadcast(OldMaxPrimaryClipAmmo, MaxPrimaryClipAmmo);
}

void AWeaponBase::OnRep_SecondaryClipAmmo(int32 OldSecondaryClipAmmo)
{
	OnSecondaryClipAmmoChanged.Broadcast(OldSecondaryClipAmmo, SecondaryClipAmmo);
}

void AWeaponBase::OnRep_MaxSecondaryClipAmmo(int32 OldMaxSecondaryClipAmmo)
{
	OnMaxSecondaryClipAmmoChanged.Broadcast(OldMaxSecondaryClipAmmo, MaxSecondaryClipAmmo);
}
