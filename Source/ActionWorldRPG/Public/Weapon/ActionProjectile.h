// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActionProjectile.generated.h"

class UProjectileMovementComponent;

UCLASS()
class ACTIONWORLDRPG_API AActionProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActionProjectile();

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "PBProjectile")
		UProjectileMovementComponent* ProjectileMovement;

};
