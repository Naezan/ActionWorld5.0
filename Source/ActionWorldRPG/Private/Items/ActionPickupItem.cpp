// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ActionPickupItem.h"

#include "Character/ActionCharacterBase.h"

#include "AbilitySystem/Abilities/ActionGameplayAbility.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"

// Sets default values
AActionPickupItem::AActionPickupItem()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bIsActive = true;
	bCanRespawn = true;
	RespawnTime = 5.0f;

	CollisionComp = CreateDefaultSubobject<UCapsuleComponent>(FName("CollisionComp"));
	CollisionComp->InitCapsuleSize(40.0f, 50.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = CollisionComp;

	RestrictedPickupTags.AddTag(FGameplayTag::RequestGameplayTag("State.Dead"));
}

void AActionPickupItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AActionPickupItem, bIsActive);
	DOREPLIFETIME(AActionPickupItem, PickedUpBy);
}

void AActionPickupItem::NotifyActorBeginOverlap(AActor* Other)
{
	if (GetLocalRole() == ROLE_Authority && Other && Other != this)
	{
		PickupOnCollision(Cast<AActionCharacterBase>(Other));
	}
}

bool AActionPickupItem::CanBePickedUp(AActionCharacterBase* Acquirer) const
{
	return bIsActive && 
	Acquirer && 
	Acquirer->IsAlive() && 
	IsValid(this) && 
	!Acquirer->GetAbilitySystemComponent()->HasAnyMatchingGameplayTags(RestrictedPickupTags) &&
	K2_CanBePickedUp(Acquirer);
}

bool AActionPickupItem::K2_CanBePickedUp_Implementation(AActionCharacterBase* Acquirer) const
{
	return true;
}

void AActionPickupItem::PickupOnCollision(AActionCharacterBase* Acquirer)
{
	if (CanBePickedUp(Acquirer))
	{
		GivePickUpTo(Acquirer);
		PickedUpBy = Acquirer;
		bIsActive = false;
		OnPickedUp();

		if (bCanRespawn && RespawnTime > 0.0f)
		{
			GetWorldTimerManager().SetTimer(
			TimerHandle_RespawnPickup, this, 
			&AActionPickupItem::RespawnPickup, RespawnTime, false);
		}
		else
		{
			Destroy();
		}
	}
}

void AActionPickupItem::GivePickUpTo(AActionCharacterBase* Acquirer)
{
	UAbilitySystemComponent* ASC = Acquirer->GetAbilitySystemComponent();

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("%s Pawn's ASC is null."), *FString(__FUNCTION__));
		return;
	}

	for (TSubclassOf<UActionGameplayAbility> Ability : GrantedAbility)
	{
		if (!Ability)
		{
			continue;
		}

		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1, static_cast<int32>(Ability.GetDefaultObject()->AbilityInputID), this);
		ASC->GiveAbilityAndActivateOnce(AbilitySpec);
	}

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	for (TSubclassOf<UGameplayEffect> Effect : GrantedEffect)
	{
		if (!Effect)
		{
			continue;
		}

		FGameplayEffectSpecHandle NewHandle = ASC->MakeOutgoingSpec(Effect, Acquirer->GetCharacterLevel(), EffectContext);

		if (NewHandle.IsValid())
		{
			ASC->ApplyGameplayEffectSpecToSelf(*NewHandle.Data.Get());
		}
	}
}

void AActionPickupItem::RespawnPickup()
{
	bIsActive = true;
	PickedUpBy = NULL;

	OnRespawned();
	TSet<AActor*> OverlappingPawns;
	GetOverlappingActors(OverlappingPawns, AActionCharacterBase::StaticClass());

	for (AActor* OverlappingPawn : OverlappingPawns)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Blue, TEXT("RespawnColl"));
		PickupOnCollision(CastChecked<AActionCharacterBase>(OverlappingPawn));
	}
}

void AActionPickupItem::OnPickedUp()
{
	//파티클 비활성화
	K2_OnPickedUp();

	if (PickUpSound && PickedUpBy)
	{
		UGameplayStatics::SpawnSoundAttached(PickUpSound, PickedUpBy->GetRootComponent());
	}
}

void AActionPickupItem::OnRespawned()
{
	//파티클 활성화
	K2_OnRespawned();
}

void AActionPickupItem::OnRep_IsActive()
{
	if (bIsActive)
	{
		//파티클 활성화
		OnRespawned();
	}
	else
	{
		//파티클 비활성화
		OnPickedUp();
	}
}
