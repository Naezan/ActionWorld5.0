// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MonsterBase.h"

//어빌리티
#include "AbilitySystem/Attributes/MonsterAttributeSet.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//컨트롤러
#include "Character/Monster/MonsterAIController.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AMonsterBase::AMonsterBase()
{
	MonsterAttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MonsterAttributeSet"));

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	//Get AIController
	MonsterController = Cast<AMonsterAIController>(GetController());

	RelativeVectorToWorldVector(PatrolPoint);

	for (FVector Vector : PatrolPoint)
	{
		DrawDebugSphere(GetWorld(), Vector, 25, 12, FColor::Red, true);
	}
}

void AMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void AMonsterBase::RelativeVectorToWorldVector(TArray<FVector>& VectorArray)
{
	for (FVector& Vector : VectorArray)
	{
		Vector = UKismetMathLibrary::TransformLocation(GetActorTransform(), Vector);
	}
}

UMonsterAttributeSet* AMonsterBase::GetMonsterAttributeSet() const
{
	return MonsterAttributeSet;
}

int32 AMonsterBase::GetCharacterLevel() const
{
	//TODO
	return 1;
}

float AMonsterBase::GetHealth() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetHealth();
	}

	return 0.0f;
}

float AMonsterBase::GetMaxHealth() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMaxHealth();
	}

	return 0.0f;
}

float AMonsterBase::GetMana() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMana();
	}
	return 0.0f;
}

float AMonsterBase::GetMaxMana() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMaxMana();
	}
	return 0.0f;
}

float AMonsterBase::GetStamina() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetStamina();
	}

	return 0.0f;
}

float AMonsterBase::GetMaxStamina() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMaxStamina();
	}

	return 0.0f;
}

float AMonsterBase::GetShield() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetShield();
	}
	return 0.0f;
}

float AMonsterBase::GetMaxShield() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMaxShield();
	}
	return 0.0f;
}

float AMonsterBase::GetMoveSpeed() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMoveSpeed();
	}

	return 0.0f;
}

float AMonsterBase::GetMoveSpeedBaseValue() const
{
	if (IsValid(MonsterAttributeSet))
	{
		return MonsterAttributeSet->GetMoveSpeedAttribute().GetGameplayAttributeData(MonsterAttributeSet)->GetBaseValue();
	}

	return 0.0f;
}

void AMonsterBase::SetHealth(float Health)
{
	if (IsValid(MonsterAttributeSet))
	{
		MonsterAttributeSet->SetHealth(Health);
	}
}

void AMonsterBase::SetMana(float Mana)
{
	if (IsValid(MonsterAttributeSet))
	{
		MonsterAttributeSet->SetMana(Mana);
	}
}

void AMonsterBase::SetStamina(float Stamina)
{
	if (IsValid(MonsterAttributeSet))
	{
		MonsterAttributeSet->SetStamina(Stamina);
	}
}

void AMonsterBase::SetShield(float Shield)
{
	if (IsValid(MonsterAttributeSet))
	{
		MonsterAttributeSet->SetShield(Shield);
	}
}