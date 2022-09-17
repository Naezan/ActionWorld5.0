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
		//�ڽ� �ݶ��̴����� ������ġ
		FVector Origin; FVector BoxExtent; float SphereRadius;
		UKismetSystemLibrary::GetComponentBounds(BoxComponent, Origin, BoxExtent, SphereRadius);

		//������ġ�� ȸ��
		FVector SpawnPoint = UKismetMathLibrary::RandomPointInBoundingBox(Origin, BoxExtent);
		SpawnPoint.Z += 100;
		FRotator SpawnRotation = FRotator(0, UKismetMathLibrary::RandomIntegerInRange(0, 360), 0);

		//�ٴ�ã��
		FHitResult HitResult; FCollisionObjectQueryParams ObjectParams;
		ObjectParams.AddObjectTypesToQuery(ECollisionChannel::ECC_WorldStatic);
		GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		SpawnPoint,
		SpawnPoint + GetActorUpVector()* -1 * 10000,
		ObjectParams);

		//������ ���� ����
		int32 SpawnMonsterIndex = UKismetMathLibrary::RandomIntegerInRange(0, Monsters.Num() - 1);

		//���� ����
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		GetWorld()->SpawnActor<AMonsterBase>(
		Monsters[SpawnMonsterIndex],
		HitResult.Location, 
		SpawnRotation,
		SpawnParams);
	}
}

