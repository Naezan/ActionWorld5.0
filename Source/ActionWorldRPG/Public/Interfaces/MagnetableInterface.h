// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MagnetableInterface.generated.h"

class USphereComponent;

// ���Ͱ� �ڼ����� �÷��̾�� ���� �� �ִ����� �Ǵ��մϴ�.
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
	//�浹�Լ� ȣ���������մϴ�.
	//BlueprintImplementable�Լ��� �������Ʈ�Լ� ȣ����� �������� ���� �����Ͱ���.
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
