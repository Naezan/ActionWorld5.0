// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Attributes/PlayerAttributeSet.h"

//서버
#include "Net/UnrealNetwork.h"

//어빌리티
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"

//캐릭터
#include "Character/ActionCharacterBase.h"
#include "Character/Player/ActionPlayerController.h"

UPlayerAttributeSet::UPlayerAttributeSet()
{
	// Cache tags
	HeadShotTag = FGameplayTag::RequestGameplayTag(FName("Effect.Damage.HeadShot"));
}

void UPlayerAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	// This is called whenever attributes change, so for max health/mana we want to scale the current totals to match
	Super::PreAttributeChange(Attribute, NewValue);

	// If a Max value changes, adjust current to keep Current % of Current to Max
	if (Attribute == GetMaxHealthAttribute()) // GetMaxHealthAttribute comes from the Macros defined at the top of the header
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		AdjustAttributeForMaxChange(Mana, MaxMana, NewValue, GetManaAttribute());
	}
	else if (Attribute == GetMaxStaminaAttribute())
	{
		AdjustAttributeForMaxChange(Stamina, MaxStamina, NewValue, GetStaminaAttribute());
	}
	else if (Attribute == GetMoveSpeedAttribute())
	{
		// Cannot slow less than 150 units/s and cannot boost more than 1000 units/s
		NewValue = FMath::Clamp<float>(NewValue, 150, 1000);
	}
}

void UPlayerAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FGameplayEffectContextHandle Context = Data.EffectSpec.GetContext();
	UAbilitySystemComponent* Source = Context.GetOriginalInstigatorAbilitySystemComponent();
	const FGameplayTagContainer& SourceTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	FGameplayTagContainer SpecAssetTags;
	Data.EffectSpec.GetAllAssetTags(SpecAssetTags);

	// Get the Target actor, which should be our owner
	AActor* TargetActor = nullptr;
	AController* TargetController = nullptr;
	AActionCharacterBase* TargetCharacter = nullptr;
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		TargetCharacter = Cast<AActionCharacterBase>(TargetActor);
	}

	// Get the Source actor
	AActor* SourceActor = nullptr;
	AController* SourceController = nullptr;
	AActionCharacterBase* SourceCharacter = nullptr;
	if (Source && Source->AbilityActorInfo.IsValid() && Source->AbilityActorInfo->AvatarActor.IsValid())
	{
		SourceActor = Source->AbilityActorInfo->AvatarActor.Get();
		SourceController = Source->AbilityActorInfo->PlayerController.Get();
		if (SourceController == nullptr && SourceActor != nullptr)
		{
			if (APawn* Pawn = Cast<APawn>(SourceActor))
			{
				SourceController = Pawn->GetController();
			}
		}

		// Use the controller to find the source pawn
		if (SourceController)
		{
			SourceCharacter = Cast<AActionCharacterBase>(SourceController->GetPawn());
		}
		else
		{
			SourceCharacter = Cast<AActionCharacterBase>(SourceActor);
		}

		// Set the causer actor based on context if it's set
		if (Context.GetEffectCauser())
		{
			SourceActor = Context.GetEffectCauser();
		}
	}

	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Store a local copy of the amount of damage done and clear the damage attribute
		const float LocalDamageDone = GetDamage();
		SetDamage(0.f);

		if (LocalDamageDone > 0.0f)
		{
			// If character was alive before damage is added, handle damage
			// This prevents damage being added to dead things and replaying death animations
			bool WasAlive = true;

			if (TargetCharacter)
			{
				WasAlive = TargetCharacter->IsAlive();
			}

			if (!TargetCharacter->IsAlive())
			{
				//UE_LOG(LogTemp, Warning, TEXT("%s() %s is NOT alive when receiving damage"), *FString(__FUNCTION__), *TargetCharacter->GetName());
			}

			// Apply the damage to shield first if it exists
			const float OldShield = GetShield();
			float DamageAfterShield = LocalDamageDone - OldShield;
			if (OldShield > 0)
			{
				float NewShield = OldShield - LocalDamageDone;
				SetShield(FMath::Clamp<float>(NewShield, 0.0f, GetMaxShield()));
			}

			if (DamageAfterShield > 0)
			{
				// Apply the health change and then clamp it
				const float NewHealth = GetHealth() - DamageAfterShield;
				SetHealth(FMath::Clamp(NewHealth, 0.0f, GetMaxHealth()));
			}

			if (TargetCharacter && WasAlive)
			{
				// This is the log statement for damage received. Turned off for live games.
				//UE_LOG(LogTemp, Log, TEXT("%s() %s Damage Received: %f"), *FString(__FUNCTION__), *GetOwningActor()->GetName(), LocalDamageDone);

				// Show damage number for the Source player unless it was self damage
				if (SourceActor != TargetActor)
				{
					AActionPlayerController* PC = Cast<AActionPlayerController>(SourceController);
					if (PC)
					{
						FGameplayTagContainer DamageNumberTags;

						if (Data.EffectSpec.GetDynamicAssetTags().HasTag(HeadShotTag))
						{
							DamageNumberTags.AddTagFast(HeadShotTag);
						}

						PC->ShowDamageNumber(LocalDamageDone, TargetCharacter, DamageNumberTags);
					}
				}

				if (!TargetCharacter->IsAlive())
				{
					// TargetCharacter was alive before this damage and now is not alive, give XP and Gold bounties to Source.
					// Don't give bounty to self.
					if (SourceController != TargetController)
					{
						// Create a dynamic instant Gameplay Effect to give the bounties
						UGameplayEffect* GEBounty = NewObject<UGameplayEffect>(GetTransientPackage(), FName(TEXT("Bounty")));
						GEBounty->DurationPolicy = EGameplayEffectDurationType::Instant;

						int32 Idx = GEBounty->Modifiers.Num();
						GEBounty->Modifiers.SetNum(Idx + 2);

						FGameplayModifierInfo& InfoXP = GEBounty->Modifiers[Idx];
						InfoXP.ModifierMagnitude = FScalableFloat(GetXPBounty());
						InfoXP.ModifierOp = EGameplayModOp::Additive;
						InfoXP.Attribute = UPlayerAttributeSet::GetXPAttribute();

						FGameplayModifierInfo& InfoGold = GEBounty->Modifiers[Idx + 1];
						InfoGold.ModifierMagnitude = FScalableFloat(GetGoldBounty());
						InfoGold.ModifierOp = EGameplayModOp::Additive;
						InfoGold.Attribute = UPlayerAttributeSet::GetGoldAttribute();

						Source->ApplyGameplayEffectToSelf(GEBounty, 1.0f, Source->MakeEffectContext());
					}
				}
			}
		}
	}// Damage
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Handle other health changes.
		// Health loss should go through Damage.
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	} // Health
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// Handle mana changes.
		SetMana(FMath::Clamp(GetMana(), 0.0f, GetMaxMana()));
	} // Mana
	else if (Data.EvaluatedData.Attribute == GetStaminaAttribute())
	{
		// Handle stamina changes.
		SetStamina(FMath::Clamp(GetStamina(), 0.0f, GetMaxStamina()));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldAttribute())
	{
		// Handle shield changes.
		SetShield(FMath::Clamp(GetShield(), 0.0f, GetMaxShield()));
	}
}

void UPlayerAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, HealthRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ManaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, StaminaRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Shield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MaxShield, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, ShieldRegenRate, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, CharacterLevel, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, XP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, XPBounty, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, Gold, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPlayerAttributeSet, GoldBounty, COND_None, REPNOTIFY_Always);
}

void UPlayerAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute, const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		// Change current value to maintain the current Val / Max percent
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f) ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}

void UPlayerAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Health, OldHealth);
}

void UPlayerAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxHealth, OldMaxHealth);
}

void UPlayerAttributeSet::OnRep_HealthRegenRate(const FGameplayAttributeData& OldHealthRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, HealthRegenRate, OldHealthRegenRate);
}

void UPlayerAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Mana, OldMana);
}

void UPlayerAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxMana, OldMaxMana);
}

void UPlayerAttributeSet::OnRep_ManaRegenRate(const FGameplayAttributeData& OldManaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ManaRegenRate, OldManaRegenRate);
}

void UPlayerAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Stamina, OldStamina);
}

void UPlayerAttributeSet::OnRep_MaxStamina(const FGameplayAttributeData& OldMaxStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxStamina, OldMaxStamina);
}

void UPlayerAttributeSet::OnRep_StaminaRegenRate(const FGameplayAttributeData& OldStaminaRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, StaminaRegenRate, OldStaminaRegenRate);
}

void UPlayerAttributeSet::OnRep_Shield(const FGameplayAttributeData& OldShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Shield, OldShield);
}

void UPlayerAttributeSet::OnRep_MaxShield(const FGameplayAttributeData& OldMaxShield)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MaxShield, OldMaxShield);
}

void UPlayerAttributeSet::OnRep_ShieldRegenRate(const FGameplayAttributeData& OldShieldRegenRate)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, ShieldRegenRate, OldShieldRegenRate);
}

void UPlayerAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Armor, OldArmor);
}

void UPlayerAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UPlayerAttributeSet::OnRep_CharacterLevel(const FGameplayAttributeData& OldCharacterLevel)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, CharacterLevel, OldCharacterLevel);
}

void UPlayerAttributeSet::OnRep_XP(const FGameplayAttributeData& OldXP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, XP, OldXP);
}

void UPlayerAttributeSet::OnRep_XPBounty(const FGameplayAttributeData& OldXPBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, XPBounty, OldXPBounty);
}

void UPlayerAttributeSet::OnRep_Gold(const FGameplayAttributeData& OldGold)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, Gold, OldGold);
}

void UPlayerAttributeSet::OnRep_GoldBounty(const FGameplayAttributeData& OldGoldBounty)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPlayerAttributeSet, GoldBounty, OldGoldBounty);
}
