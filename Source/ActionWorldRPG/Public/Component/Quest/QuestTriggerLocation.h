// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Component/Quest/QuestTriggerBase.h"
#include "QuestTriggerLocation.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)//�������Ʈ�� ������ ���, �������Ʈ�� ��������
class ACTIONWORLDRPG_API AQuestTriggerLocation : public AQuestTriggerBase
{
	GENERATED_BODY()
	
public:
    AQuestTriggerLocation();

	UFUNCTION()
		void OnBeginOverlap(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		);

protected:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray<AActor*> MonsterSpawners;

    virtual void BeginPlay() override;
};
