// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/MagnetableInterface.h"
#include "Components/TimeLineComponent.h"
#include "CollectableItem.generated.h"

//수집가능한 아무런 특성이 없는 기본액터입니다.
//기본적으로 자성을 통해서 수집이 가능하며 아이템스포너컴포넌트에 의해서만 레벨에 배치될 수 있습니다.
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
	//타임라인
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
