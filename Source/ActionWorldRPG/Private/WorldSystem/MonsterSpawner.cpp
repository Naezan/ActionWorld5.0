// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldSystem/MonsterSpawner.h"

#include "Character/Monster/MonsterBase.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterSpawner::WorldInteractionProcess_Implementation()
{
	for (int i = 0; i < MaxMonsterCount; ++i)
	{
		//박스 콜라이더에서 램덤위치
		FVector Origin; FVector BoxExtent; float SphereRadius;
		UKismetSystemLibrary::GetComponentBounds(BoxComponent, Origin, BoxExtent, SphereRadius);

		//스폰위치와 회전
		FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
		SpawnPoint.Z += 100;
		FRotator SpawnRotation = FRotator(0, UKismetMathLibrary::RandomIntegerInRange(0, 360), 0);

		//바닥찾기
		FHitResult HitResult; FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		SpawnPoint,
		SpawnPoint + GetActorUpVector()* -1 * 10000,
		ObjectParams);

		//스폰할 액터 선택
		int32 SpawnMonsterIndex = UKismetMathLibrary::RandomIntegerInRange(0, Monsters.Num() - 1);

		//액터 스폰
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AMonsterBase>(
		Monsters[SpawnMonsterIndex],
		HitResult.Location, 
		SpawnRotation,
		SpawnParams);
	}
}

