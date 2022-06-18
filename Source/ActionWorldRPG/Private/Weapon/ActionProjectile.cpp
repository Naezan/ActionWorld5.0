// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/ActionProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AActionProjectile::AActionProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->ProjectileGravityScale = 0;
	ProjectileMovement->InitialSpeed = 7000.0f;

	bReplicates = true;

	//TODO change this to a better value
	NetUpdateFrequency = 100.0f;
}
