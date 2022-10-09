// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MonsterBase.h"
#include "Character/Player/PlayerBase.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//컨트롤러
#include "Character/Monster/MonsterAIController.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

//AI
#include "Components/CapsuleComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Touch.h"

AMonsterBase::AMonsterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}

void AMonsterBase::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	//AI의 Touch인식용
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &ThisClass::CapsuleHit);

	RelativeVectorToWorldVector(PatrolPoint);

	for (const FVector& Vector : PatrolPoint)
	{
		DrawDebugSphere(GetWorld(), Vector, 25, 12, FColor::Red, true);
	}
}

void AMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AMonsterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//Get AIController
	MonsterController = Cast<AMonsterAIController>(GetController());

	if (IsValid(DefaultAttributeSet))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("ValidAttribute"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, TEXT("NotValidAttribute"));
	}
}

void AMonsterBase::RelativeVectorToWorldVector(TArray<FVector>& VectorArray)
{
	for (FVector& Vector : VectorArray)
	{
		Vector = UKismetMathLibrary::TransformLocation(GetActorTransform(), Vector);
	}
}

void AMonsterBase::CapsuleHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (MonsterController != nullptr)
	{
		if (APlayerBase* Player = Cast<APlayerBase>(OtherActor))
		{
			UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(this);
			if (PerceptionSystem)
			{
				FAITouchEvent Event(MonsterController, Player, Hit.ImpactPoint);
				PerceptionSystem->OnEvent(Event);
			}
		}
	}
}
