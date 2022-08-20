// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/MagnetableInterface.h"
#include "Components/TimeLineComponent.h"
#include "CollectableItem.generated.h"

//���������� �ƹ��� Ư���� ���� �⺻�����Դϴ�.
//�⺻������ �ڼ��� ���ؼ� ������ �����ϸ� �����۽�����������Ʈ�� ���ؼ��� ������ ��ġ�� �� �ֽ��ϴ�.
UCLASS()
class ACTIONWORLDRPG_API ACollectableItem : public AActor, public IMagnetableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACollectableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	class UItemCollectorComponent* ItemCollectorComponent;

	UPROPERTY()
	AActor* FollowTarget;

public:
	UFUNCTION()
		virtual void OnBeginOverlap(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult
		) override;

	UFUNCTION()
		virtual void OnEndOverlap(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex
		) override;

		UFUNCTION(BlueprintImplementableEvent)
		void FinishCollect();

public:
	//Ÿ�Ӷ���
	UPROPERTY()
	UTimelineComponent* FollowTargetTimeline;
	UPROPERTY(EditAnywhere, Category = "Timeline")
	UCurveFloat* FollowTargetCurveFloat;

	FOnTimelineFloat FollowTargetFunction;
	FOnTimelineEvent FollowTargetFinishEvent;

	UFUNCTION()
	void InitFollowTargetTimeline();

	UFUNCTION()
	void OnMoveToTarget(float DeltaTime);
	UFUNCTION()
	void OnMoveFinished();
};
