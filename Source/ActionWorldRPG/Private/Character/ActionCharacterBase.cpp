// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ActionCharacterBase.h"
#include "..\..\Public\Character\ActionCharacterBase.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/ActionGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"

//게임피쳐
#include "Components/GameFrameworkComponentManager.h"

//캐릭터
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "AbilitySystem/Attributes/PlayerAttributeSet.h"
#include "AbilitySystem/Attributes/AmmoAttributeSet.h"

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
AActionCharacterBase::AActionCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UActionAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	//UE_LOG(LogTemp, Warning, TEXT("AbilitySystemSet"));
	AbilitySystemComponent->SetIsReplicated(true);

	DefaultAttributeSet = CreateDefaultSubobject<UPlayerAttributeSet>(TEXT("DefaultAttributeSet"));
	AmmoAttributeSet = CreateDefaultSubobject<UAmmoAttributeSet>(TEXT("AmmoAttributeSet"));

	//체력, 총알 델리게이트 함수 바인딩
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UPlayerAttributeSet::GetManaAttribute()).AddUObject(this, &ThisClass::HandleManaChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetRifleReserveAmmoAttribute()).AddUObject(this, &ThisClass::HandleRifleReserveAmmoChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UAmmoAttributeSet::GetPistolReserveAmmoAttribute()).AddUObject(this, &ThisClass::HandlePistolReserveAmmoChanged);
	//Clip총알들도 바인딩해줘야하나?..매순간브로드캐스팅되면 어떻게될까?

	// Cache tags
	DeadTag = FGameplayTag::RequestGameplayTag("State.Dead");
	EffectRemoveOnDeathTag = FGameplayTag::RequestGameplayTag("Effect.RemoveOnDeath");

	RagdollProfileName = FName("Ragdoll");
	RagdollImpulseBone = FName("Pelvis");
	RadgollImpulsePower = 500.f;
}

void AActionCharacterBase::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AActionCharacterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	//어빌리티관련셋팅은 다음틱이후에 하드코딩
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("OnInitializeAbilitySystem"));
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
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
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// If we handle players disconnecting and rejoining in the future, we'll have to change this so that possession from rejoining doesn't reset attributes.
		// For now assume possession = spawn/respawn.
		InitializeAttributes();

		AddStartupEffects();

		AddCharacterAbilities();

		//어빌리티관련셋팅은 다음틱이후에 하드코딩
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUFunction(this, FName("OnInitializeAbilitySystem"));
		GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);

		// Remove Dead tag
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(DeadTag));
	}
}

void AActionCharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AActionCharacterBase::OnRep_Controller()
{
	Super::OnRep_Controller();
}

void AActionCharacterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnitializeAbilitySystem();

	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
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

int32 AActionCharacterBase::GetAbilityLevel(EActionAbilityInputID AbilityID) const
{
	return 1;
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

bool AActionCharacterBase::ActivateAbilityWithTags(FGameplayTagContainer AbilityTags, bool bAllowRemoteActivation)
{
	if (AbilitySystemComponent)
	{
		return AbilitySystemComponent->TryActivateAbilitiesByTag(AbilityTags, bAllowRemoteActivation);
	}

	return false;
}

bool AActionCharacterBase::IsAlive() const
{
	return GetHealth() > 0.0f;
}

void AActionCharacterBase::StartDeath()
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("StartDeath"));
	K2_OnDestroyStarted();
	PlayDeathAnimationAndSound();
	Ragdoll();
	DisableMovementAndCollision();
	HideEquippedWeapon();

	OnDeathStarted.Broadcast(this);
}

void AActionCharacterBase::FinishDeath()
{
	OnDeathFinished.Broadcast(this);

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::OnDestroyFinished);
}

void AActionCharacterBase::ShowDamageNumber()
{
	//TODO
}

void AActionCharacterBase::AddDamageNumber(float Damage, FGameplayTagContainer DamageNumberTags)
{
	DamageNumberQueue.Add(FActionDamageNumber(Damage, DamageNumberTags));

	if (!GetWorldTimerManager().IsTimerActive(DamageNumberTimer))
	{
		GetWorldTimerManager().SetTimer(DamageNumberTimer, this, &AActionCharacterBase::ShowDamageNumber, 0.1, true, 0.0f);
	}
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

void AActionCharacterBase::OnInitializeAbilitySystem()
{
	DefaultAttributeSet->OnZeroHealth.AddUObject(this, &ThisClass::HandleZeroHealth);

	//어트리뷰트정보 초기화(체력, 마나, 라이플총알들, 권총 총알들)
	AbilitySystemComponent->SetNumericAttributeBase(UPlayerAttributeSet::GetHealthAttribute(), DefaultAttributeSet->GetMaxHealth());
	AbilitySystemComponent->SetNumericAttributeBase(UPlayerAttributeSet::GetManaAttribute(), DefaultAttributeSet->GetMaxMana());
	AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetRifleReserveAmmoAttribute(), AmmoAttributeSet->GetMaxRifleReserveAmmo());
	//AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetRifleClipAmmoAttribute(), AmmoAttributeSet->GetMaxRifleClipAmmo());
	AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetPistolReserveAmmoAttribute(), AmmoAttributeSet->GetMaxPistolReserveAmmo());
	//AbilitySystemComponent->SetNumericAttributeBase(UAmmoAttributeSet::GetPistolClipAmmoAttribute(), AmmoAttributeSet->GetMaxPistolClipAmmo());

	//UI리셋?
	OnHealthChanged.Broadcast(this, DefaultAttributeSet->GetHealth(), DefaultAttributeSet->GetHealth());
	OnManaChanged.Broadcast(this, DefaultAttributeSet->GetMana(), DefaultAttributeSet->GetMana());
	OnRifleAmmoChanged.Broadcast(this, AmmoAttributeSet->GetRifleReserveAmmo(), AmmoAttributeSet->GetRifleReserveAmmo());
	OnPistolAmmoChanged.Broadcast(this, AmmoAttributeSet->GetPistolReserveAmmo(), AmmoAttributeSet->GetPistolReserveAmmo());
}

void AActionCharacterBase::UnitializeAbilitySystem()
{
	if (DefaultAttributeSet)
	{
		DefaultAttributeSet->OnZeroHealth.RemoveAll(this);
	}

	DefaultAttributeSet = nullptr;
	AmmoAttributeSet = nullptr;
	AbilitySystemComponent = nullptr;
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

void AActionCharacterBase::HandleZeroHealth(AActor* DamageInstigator, const FGameplayEffectSpec& DamageEffectSpec, float DamageAmount)
{
	if (AbilitySystemComponent)
	{
		FGameplayEventData PayLoad;
		PayLoad.EventTag = FGameplayTag::RequestGameplayTag(FName("GameplayEvent.Death"));//하드코딩
		PayLoad.Instigator = DamageInstigator;
		PayLoad.Target = AbilitySystemComponent->GetAvatarActor();
		PayLoad.OptionalObject = DamageEffectSpec.Def;
		PayLoad.ContextHandle = DamageEffectSpec.GetEffectContext();
		PayLoad.InstigatorTags = *DamageEffectSpec.CapturedSourceTags.GetAggregatedTags();
		PayLoad.TargetTags = *DamageEffectSpec.CapturedTargetTags.GetAggregatedTags();
		PayLoad.EventMagnitude = DamageAmount;

		//데미지를 준대상이 누구인지에 대한정보를 포함한 값으로 이벤트를 타겟인 자신에게 전달한다.
		//자신이 어빌리티태그와 관련된 어빌리티를가지고 있다면 호출된다.
		AbilitySystemComponent->HandleGameplayEvent(PayLoad.EventTag, &PayLoad);
	}
}

void AActionCharacterBase::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	//GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("HandleHealthChanged"));
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void AActionCharacterBase::HandleManaChanged(const FOnAttributeChangeData& ChangeData)
{
	OnManaChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void AActionCharacterBase::HandleRifleReserveAmmoChanged(const FOnAttributeChangeData& ChangeData)
{
	OnRifleAmmoChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
}

void AActionCharacterBase::HandlePistolReserveAmmoChanged(const FOnAttributeChangeData& ChangeData)
{
	OnPistolAmmoChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue);
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

void AActionCharacterBase::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	if (CapsuleComp == nullptr)
	{
		return;
	}

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
}

void AActionCharacterBase::PlayDeathAnimationAndSound()
{
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	}

	if (!DeathMontage.IsEmpty())
	{
		int32 index = UKismetMathLibrary::RandomIntegerInRange(0, DeathMontage.Num() - 1);
		PlayAnimMontage(DeathMontage[index]);
	}
}

void AActionCharacterBase::Ragdoll()
{
	FTimerHandle WaitHandle;
	float WaitTime = UKismetMathLibrary::RandomFloatInRange(0.1f, 0.2f);
	GetWorld()->GetTimerManager().SetTimer(WaitHandle, FTimerDelegate::CreateLambda([&]()
		{
			ApplyPhysics();
		}), WaitTime, false);
}

void AActionCharacterBase::ApplyPhysics()
{
	GetMesh()->SetCollisionProfileName(RagdollProfileName);
	GetMesh()->SetAllBodiesBelowSimulatePhysics(RagdollImpulseBone, true);

	//FVector Impulse = GetCharacterMovement()->GetLastUpdateVelocity();
	//GetMesh()->AddImpulseAtLocation(Impulse, GetActorLocation(), RagdollImpulseBone);
	//GetMesh()->AddImpulse(Impulse + Impulse.GetSafeNormal() * RadgollImpulsePower, RagdollImpulseBone, true);
}

void AActionCharacterBase::HideEquippedWeapon()
{
	//TO DO
}

void AActionCharacterBase::OnDestroyFinished()
{
	K2_OnDestroyFinished();

	DestroyWhenFinishDeath();
}

void AActionCharacterBase::DestroyWhenFinishDeath()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FinishDying();
		//액터가 사라지기전에 안정적으로 컨트롤러 제거
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	//FinishDying();

	SetActorHiddenInGame(true);
}

void AActionCharacterBase::FinishDying()
{
	RemoveCharacterAbilities();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->CancelAllAbilities();
		//AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();

		FGameplayTagContainer EffectTagsToRemove;
		EffectTagsToRemove.AddTag(EffectRemoveOnDeathTag);
		int32 NumEffectsRemoved = AbilitySystemComponent->RemoveActiveEffectsWithTags(EffectTagsToRemove);

		//플레이어 사망쪽 DeadthEffect가 유효할까?
		AbilitySystemComponent->ApplyGameplayEffectToSelf(Cast<UGameplayEffect>(DeathEffect->GetDefaultObject()), 1.0f, AbilitySystemComponent->MakeEffectContext());

		AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
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
