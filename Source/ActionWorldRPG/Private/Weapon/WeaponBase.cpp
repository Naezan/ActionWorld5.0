// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/WeaponBase.h"

//캐릭터
#include "Character/Player/PlayerBase.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ActionGameplayAbility.h"
#include "AbilitySystem/GATA_LineTrace.h"

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

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(CollisionComp);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponMesh->SetRelativeLocation(WeaponMesh3PickupRelativeLocation);

	// Set this actor to never tick
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	NetUpdateFrequency = 100.0f; // Set this to a value that's appropriate for your game
	bSpawnWithCollision = true;
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

USkeletalMeshComponent* AWeaponBase::GetWeaponMesh() const
{
	return WeaponMesh;
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AWeaponBase, OwningCharacter, COND_OwnerOnly);
}

void AWeaponBase::PreReplication(IRepChangedPropertyTracker& ChangedPropertyTracker)
{
	Super::PreReplication(ChangedPropertyTracker);
}

void AWeaponBase::SetOwningCharacter(APlayerBase* InOwningCharacter)
{
	OwningCharacter = InOwningCharacter;
	if (OwningCharacter)
	{
		// Called when added to inventory
		AbilitySystemComponent = Cast<UActionAbilitySystemComponent>(OwningCharacter->GetAbilitySystemComponent());
		SetOwner(InOwningCharacter);
		//AttachToComponent(OwningCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		CollisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		AbilitySystemComponent = nullptr;
		SetOwner(nullptr);
		//DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

void AWeaponBase::AddWeaponInfoOnInteract(AActor* Other)
{
	if (IsValid(this) && !OwningCharacter)
	{
		PickUpOnInteract(Cast<APlayerBase>(Other));
	}
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
			FGameplayAbilitySpec(Ability, GetAbilityLevel(Ability.GetDefaultObject()->AbilityID),
				static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this)));
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

bool AWeaponBase::HasInfiniteAmmo() const
{
	return bInfiniteAmmo;
}

UAnimMontage* AWeaponBase::GetEquipMontage() const
{
	return EquipMontage;
}

USoundCue* AWeaponBase::GetPickupSound() const
{
	return PickupSound;
}

FText AWeaponBase::GetDefaultStatusText() const
{
	return DefaultStatusText;
}

AGATA_LineTrace* AWeaponBase::GetLineTraceTargetActor()
{
	if (LineTraceTargetActor)
	{
		return LineTraceTargetActor;
	}

	LineTraceTargetActor = OwningCharacter->GetWorld()->SpawnActor<AGATA_LineTrace>();
	LineTraceTargetActor->SetOwner(this);
	return LineTraceTargetActor;
}

void AWeaponBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	if (LineTraceTargetActor)
	{
		LineTraceTargetActor->Destroy();
	}
	/*
	if (SphereTraceTargetActor)
	{
		SphereTraceTargetActor->Destroy();
	}*/

	Super::EndPlay(EndPlayReason);
}

void AWeaponBase::PickUpOnInteract(APlayerBase* InCharacter)
{
	if (!InCharacter || !InCharacter->IsAlive() || !InCharacter->GetAbilitySystemComponent() || InCharacter->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags))
	{
		return;
	}

	if (InCharacter->AddWeaponToInventory(this, true)/* && OwningCharacter->IsInFirstPersonPerspective()*/)
	{
		//무기 발사모드 설정
		FireMode = DefaultFireMode;

		/*WeaponMesh->CastShadow = false;
		WeaponMesh->SetVisibility(true, true);
		WeaponMesh->SetVisibility(false, true);*/
	}
}