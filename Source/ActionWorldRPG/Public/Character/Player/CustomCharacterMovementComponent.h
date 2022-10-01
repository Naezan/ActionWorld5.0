// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ObjectMacros.h"
#include "CustomCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_Climbing	UMETA(DisplayName = "Climbing"),
	CMOVE_MAX		UMETA(Hidden),
};

UCLASS()
class ACTIONWORLDRPG_API UCustomCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

		UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

private:
	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//무브먼트 모드를 변경합니다.
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	//캡슐의 크기조절기능이 들어있습니다.
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	//virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	void PhysClimbing(float deltaTime, int32 Iterations);//

	virtual float GetMaxSpeed() const override;
	virtual float GetMaxAcceleration() const override;
public:
	virtual bool IsCrouching() const override;
	virtual bool IsFalling() const override;

	void TryClimbing();

	void CancelClimbing();

	UFUNCTION(BlueprintPure)
	bool IsClimbing() const;

	UFUNCTION(BlueprintPure)
	FVector GetClimbSurfaceNormal() const;

	FORCEINLINE bool GetIsJumpClimb() const { return IsJumpClimb; };

	//처음 클라이밍상태라면 true 이후에는 모두 false입니다.
	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		bool bIsPlayClimbStart = true;

	//이전에 클라이밍 상태였는지여부
	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		bool bIsPrevClambing;

	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		TArray<UAnimMontage*> ClimbJumpMontages;//왼위, 위, 오위 | 왼, X, 오 | 왼아, 아, 오아 || 행이 Forward, 열이 Right

private:
	//매달릴 수 있는위치를 찾는 함수
	UFUNCTION(BlueprintCallable)
	bool SphereTraceAndStoreWallHit();

	//매달릴 수 있는지 여부 -> 이경우에는 모든벽의 기울기를 통해서 계산하지만 특정 매달리는 지점을 정의한다면 필요없는 부분이다.
	bool CanStartClimbing();

	//플레이어의 눈보다 위에 있는지 여부 중요하지만 현재 내가 만들고 싶은 기능에선 단순히 캡슐콜리전의 위치만 올리면 된다.
	bool EyeHeightTrace(const float TraceDistance) const;

	//눈앞에서 탐지가 되는지 평면일수록 짧게 경사질수록 길게 라인을 그려서 체크
	bool IsFacingSurface(const float Steepness) const;
	UFUNCTION(BlueprintCallable)
	void OnEndClimbMove();

	//피직스 클리이밍 함수
	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float deltaTime);
	//모든 움직임계산을 하기전에 이 함수를 무조건 호출해야합니다.
	bool ShouldStopClimbing();
	void StopClimbing(float deltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float deltaTime);
	void SnapToClimbingSurface(float deltaTime) const;

	FQuat GetClimbingRotation(float deltaTime) const;
	//피직스 클리이밍 함수

	//땅에 거의 가깝게 내려왔을때 호출됩니다.
	bool ClimbDownToFloor() const;

	bool CheckFloor(FHitResult& FloorHit) const;


	//클라이밍 렛지
	UFUNCTION(BlueprintCallable)
	bool TryClimbUpLedge(float ForwardValue);
	//착지가 가능한 Top부분에 장애물이 없는지 확인합니다. 장애물이 없으면 true를 반환합니다.
	bool HasReachedEdge() const;
	//지면이 걸을 수 있는지 확인합니다.
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	//캐릭터가 올라갈 공간이 있는지 확인합니다.
	bool CanMoveToLedgeClimbLocation() const;
	//클라이밍 렛지

	//인풋 클라이밍
	UFUNCTION(BlueprintCallable)
	bool CalculateJumpClimbPoint(float ForwardValue, float RightValue);
	UFUNCTION(BlueprintCallable)
	//위치, 로테이션 셋팅
	void SetWarpJumpClimbValue(float ForwardValue, float RightValue);
	//인풋 클라이밍


	//잡을지점 체크 변수
	UPROPERTY(Category = "Climbing", EditAnywhere)
		int32 CollisionCapsuleRadius = 60;
	//잡을지점 체크 변수
	UPROPERTY(Category = "Climbing", EditAnywhere)
		int32 CollisionCapsuleHalfHeight = 72;

	//현재 탐지한 벽
	FHitResult CurrentWallHit;
	FHitResult CurrentLedgeHit;

	FHitResult CurrentJumpClimbHit;
	FHitResult CurrentJumpClimbWallHit;

	FCollisionQueryParams ClimbQueryparams;

	//잡을 수 있는 벽의 기울기 사용안할것이다
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "75.0"))
		float MinHorizontalDegreesToStartClimbing = 25;

	bool bWantToClimb = false;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "72.0"))
		float ClimbingCollisionShrinkAmount = 30;

	//피직스 클라이밍변수
	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"))
		float MaxClimbingSpeed = 120.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"))
		float MaxClimbingAcceleration = 380.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "3000.0"))
		float BrakingDecelerationClimbing = 550.f;

	//클라이밍 로테이션
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "12.0"))
		int ClimbingRotationSpeed = 6;

	//스냅 무브먼트
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "60.0"))
		float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
		float DistanceFromSurface = 55.f;

	//1미터거리에서 떨어지면 제자리 착지모션을 재생합니다.
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "500.0"))
		float FloorCheckDistance = 100.f;

	//LedgeAnimation
	UPROPERTY(Category = "Climbing", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* LedgeClimbMontage;

	UPROPERTY(Category = "Climbing", EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* JumpClimbFailMontage;

	UPROPERTY()
		UAnimInstance* AnimInstance;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "150"))
		float LedgeClimbEyeShrinkAmount = 60.f;
	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsLedgeClimbEnd;

	//클라이밍 점프 모션워핑 변수
	UPROPERTY(Category = "Climbing MotionWarping", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsJumpClimb;

	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector WarpJumpClimbingPoint;
	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator WarpJumpClimbingRotation;

	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* JumpClimbMontage;
};
