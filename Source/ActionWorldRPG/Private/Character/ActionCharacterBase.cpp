// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ActionCharacterBase.h"
#include "..\..\Public\Character\ActionCharacterBase.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ActionGameplayAbility.h"
#include "AbilitySystemGlobals.h"

//캐릭터
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystem/Attributes/PlayerAttributeSet.h"
#include "AbilitySystem/Attributes/AmmoAttributeSet.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AActionCharacterBase::AActionCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UActionAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//UE_LOG(LogTemp, Warning, TEXT("AbilitySystemSet"));
	AbilitySystemComponent->SetIsReplicated(true);

	DefaultAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("DefaultAttributeSet"));
	AmmoAttributeSet = CreateDefaultSubobject<UAmmoAttributeSet>(TEXT("AmmoAttributeSet"));

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag("Effect.RemoveOnDeath");

}

// Called when the game starts or when spawned
void AActionCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AActionCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Initialize our abilities
	if (AbilitySystemComponent)
	{
		// AI won't have PlayerControllers so we can init again here just to be sure. No harm in initing twice for heroes that have PlayerControllers.
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("StartUpAbility"));

		//UE_LOG(LogTemp, Warning, TEXT("%d"), AbilitySystemComponent->GetActivatableAbilities().Num());

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
	}
}

UActionAbilitySystemComponent* AActionCharacterBase::GetActionAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAbilitySystemComponent* AActionCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
UPlayerAttributeSet* AActionCharacterBase::GetDefaultAttributeSet() const
{
	return DefaultAttributeSet;
}
UAmmoAttributeSet* AActionCharacterBase::GetAmmoAttributeSet() const
{
	return AmmoAttributeSet;
}

bool AActionCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

int32 AActionCharacterBase::GetAbilityLevel(EActionAbilityInputID AbilityID) const
{
	return 1;
}

void AActionCharacterBase::Die()
{
	// Only runs on Server
	RemoveCharacterAbilities();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->GravityScale = 0;
	GetCharacterMovement()->Velocity = FVector(0);

	OnCharacterDied.Broadcast(this);

	if (IsValid(AbilitySystemComponent))
	{
		AbilitySystemComponent->CancelAllAbilities();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
	}

	//TODO replace with a locally executed GameplayCue
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (DeathMontage)
	{
		PlayAnimMontage(DeathMontage);
	}
	else
	{
		FinishDying();
	}
}

void AActionCharacterBase::FinishDying()
{
	K2_Die();

	SetLifeSpan(0.1f);
}

void AActionCharacterBase::AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags)
{
	DamageNumberQueue.Add(FActionDamageNumber(Damage, DamageNumberTags));

	if (!GetWorldTimerManager().IsTimerActive(DamageNumberTimer))
	{
		GetWorldTimerManager().SetTimer(DamageNumberTimer, this, &AActionCharacterBase::ShowDamageNumber, 0.1, true, 0.0f);
	}
}

bool AActionCharacterBase::ActivateAbilityWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

int32 AActionCharacterBase::GetCharacterLevel() const
{
	//TODO
	return 1;
}

float AActionCharacterBase::GetHealth() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetHealth();
	}

	return 0.0f;
}

float AActionCharacterBase::GetMaxHealth() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMaxHealth();
	}

	return 0.0f;
}

float AActionCharacterBase::GetMana() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMana();
	}
	return 0.0f;
}

float AActionCharacterBase::GetMaxMana() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMaxMana();
	}
	return 0.0f;
}

float AActionCharacterBase::GetStamina() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetStamina();
	}

	return 0.0f;
}

float AActionCharacterBase::GetMaxStamina() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMaxStamina();
	}

	return 0.0f;
}

float AActionCharacterBase::GetShield() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetShield();
	}
	return 0.0f;
}

float AActionCharacterBase::GetMaxShield() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMaxShield();
	}
	return 0.0f;
}

float AActionCharacterBase::GetMoveSpeed() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMoveSpeed();
	}

	return 0.0f;
}

float AActionCharacterBase::GetMoveSpeedBaseValue() const
{
	if (IsValid(DefaultAttributeSet))
	{
		return DefaultAttributeSet->GetMoveSpeedAttribute().GetGameplayAttributeData(DefaultAttributeSet)->GetBaseValue();
	}

	return 0.0f;
}

void AActionCharacterBase::RemoveCharacterAbilities()
{
	//어빌리티를 받은적이 없으면 리턴
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || !AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	// Remove any abilities added from a previous call. This checks to make sure the ability is in the startup 'CharacterAbilities' array.
	TArray<FGameplayAbilitySpecHandle> AbilitiesToRemove;
	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	{
		if ((Spec.SourceObject == this) && CharacterAbilities.Contains(Spec.Ability->GetClass()))
		{
			AbilitiesToRemove.Add(Spec.Handle);
		}
	}

	// Do in two passes so the removal happens after we have the full list
	for (int32 i = 0; i < AbilitiesToRemove.Num(); i++)
	{
		AbilitySystemComponent->ClearAbility(AbilitiesToRemove[i]);
	}

	//어빌리티 상태를 안받은상태로 바꿈
	AbilitySystemComponent->CharacterAbilitiesGiven = false;
}

void AActionCharacterBase::AddCharacterAbilities()
{
	// Grant abilities, but only on the server	
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->CharacterAbilitiesGiven)
	{
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("%s() %s Damage Received"), *FString(__FUNCTION__), *GetName());

	for (TSubclassOf<UActionGameplayAbility>& StartupAbility : CharacterAbilities)
	{
		AbilitySystemComponent->GiveAbility(
			FGameplayAbilitySpec(StartupAbility, 
			GetAbilityLevel(StartupAbility.GetDefaultObject()->AbilityID), 
			static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s() %s AbilityActivated"), *FString(__FUNCTION__), *GetName());

	AbilitySystemComponent->CharacterAbilitiesGiven = true;
}

void AActionCharacterBase::InitializeAttributes()
{
	if (!IsValid(AbilitySystemComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing AbilitySystemComponent for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	if (!DefaultAttributes)
	{
		UE_LOG(LogTemp, Error, TEXT("%s() Missing DefaultAttributes for %s. Please fill in the character's Blueprint."), *FString(__FUNCTION__), *GetName());
		return;
	}

	// Can run on Server and Client
	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributes, GetCharacterLevel(), EffectContext);
	if (NewHandle.IsValid())
	{
		FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
	}
}

void AActionCharacterBase::AddStartupEffects()
{
	if (GetLocalRole() != ROLE_Authority || !IsValid(AbilitySystemComponent) || AbilitySystemComponent->StartupEffectsApplied)
	{
		return;
	}

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> GameplayEffect : StartupEffects)
	{
		FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffect, GetCharacterLevel(), EffectContext);
		if (NewHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*NewHandle.Data.Get(), AbilitySystemComponent);
		}
	}

	AbilitySystemComponent->StartupEffectsApplied = true;
}

void AActionCharacterBase::ShowDamageNumber()
{
	//TODO
}

void AActionCharacterBase::SetHealth(float Health)
{
	if (IsValid(DefaultAttributeSet))
	{
		DefaultAttributeSet->SetHealth(Health);
	}
}

void AActionCharacterBase::SetMana(float Mana)
{
	if (IsValid(DefaultAttributeSet))
	{
		DefaultAttributeSet->SetMana(Mana);
	}
}

void AActionCharacterBase::SetStamina(float Stamina)
{
	if (IsValid(DefaultAttributeSet))
	{
		DefaultAttributeSet->SetStamina(Stamina);
	}
}

void AActionCharacterBase::SetShield(float Shield)
{
	if (IsValid(DefaultAttributeSet))
	{
		DefaultAttributeSet->SetShield(Shield);
	}
}

void AActionCharacterBase::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	if (HasAuthority())
	{
		MyTeamID = NewTeamID;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot set team for %s on non-authority"), *GetPathName(this));
	}
}

FGenericTeamId AActionCharacterBase::GetGenericTeamId() const
{
	return MyTeamID;
}
