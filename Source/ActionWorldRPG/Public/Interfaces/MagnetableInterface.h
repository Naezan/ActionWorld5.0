// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MagnetableInterface.generated.h"

class USphereComponent;

// 액터가 자성으로 플레이어에게 붙을 수 있는지를 판단합니다.
UINTERFACE(MinimalAPI)
class UMagnetableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ACTIONWORLDRPG_API IMagnetableInterface
{
	GENERATED_BODY()

public:
	//충돌함수 호출을강제합니다.
	//BlueprintImplementable함수로 블루프린트함수 호출까지 생각해줄 수도 있을것같다.
	UFUNCTION()
	virtual void OnBeginOverlap(
	class UPrimitiveComponent* OverlappedComp, 
	class AActor* OtherActor, 
	class UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult
	) = 0;

	UFUNCTION()
		virtual void OnEndOverlap(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex) = 0;

protected:
	USphereComponent* SphereComponent;
};
