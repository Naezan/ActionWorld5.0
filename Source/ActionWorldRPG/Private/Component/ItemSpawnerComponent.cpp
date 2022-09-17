// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/ItemSpawnerComponent.h"

// Sets default values for this component's properties
UItemSpawnerComponent::UItemSpawnerComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UItemSpawnerComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UItemSpawnerComponent::SpawnLoot(UClass* ItemClass, FTransform const& Transform, int32 ItemCount)
{
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	//20%Ȯ���� źâ, 30%Ȯ���� ü�¶Ǵ� ����, 5%Ȯ���� ����

	for (int32 i = 0; i < ItemCount; ++i)
	{
		GetWorld()->SpawnActor(ItemClass, &Transform, SpawnParameters);
	}
}
