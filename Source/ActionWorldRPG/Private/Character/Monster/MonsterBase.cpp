// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MonsterBase.h"

//어빌리티
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//컨트롤러
#include "Character/Monster/MonsterAIController.h"

#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"

AMonsterBase::AMonsterBase()
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