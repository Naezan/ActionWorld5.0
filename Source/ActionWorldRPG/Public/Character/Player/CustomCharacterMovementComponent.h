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
	//�����Ʈ ��带 �����մϴ�.
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	//ĸ���� ũ����������� ����ֽ��ϴ�.
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

	//ó�� Ŭ���ֻ̹��¶�� true ���Ŀ��� ��� false�Դϴ�.
	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		bool bIsPlayClimbStart = true;

	//������ Ŭ���̹� ���¿���������
	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		bool bIsPrevClambing;

	UPROPERTY(Category = "Climbing", EditAnywhere, BlueprintReadWrite)
		TArray<UAnimMontage*> ClimbJumpMontages;//����, ��, ���� | ��, X, �� | �޾�, ��, ���� || ���� Forward, ���� Right

private:
	//�Ŵ޸� �� �ִ���ġ�� ã�� �Լ�
	UFUNCTION(BlueprintCallable)
	bool SphereTraceAndStoreWallHit();

	//�Ŵ޸� �� �ִ��� ���� -> �̰�쿡�� ��纮�� ���⸦ ���ؼ� ��������� Ư�� �Ŵ޸��� ������ �����Ѵٸ� �ʿ���� �κ��̴�.
	bool CanStartClimbing();

	//�÷��̾��� ������ ���� �ִ��� ���� �߿������� ���� ���� ����� ���� ��ɿ��� �ܼ��� ĸ���ݸ����� ��ġ�� �ø��� �ȴ�.
	bool EyeHeightTrace(const float TraceDistance) const;

	//���տ��� Ž���� �Ǵ��� ����ϼ��� ª�� ��������� ��� ������ �׷��� üũ
	bool IsFacingSurface(const float Steepness) const;
	UFUNCTION(BlueprintCallable)
	void OnEndClimbMove();

	//������ Ŭ���̹� �Լ�
	void ComputeSurfaceInfo();
	void ComputeClimbingVelocity(float deltaTime);
	//��� �����Ӱ���� �ϱ����� �� �Լ��� ������ ȣ���ؾ��մϴ�.
	bool ShouldStopClimbing();
	void StopClimbing(float deltaTime, int32 Iterations);
	void MoveAlongClimbingSurface(float deltaTime);
	void SnapToClimbingSurface(float deltaTime) const;

	FQuat GetClimbingRotation(float deltaTime) const;
	//������ Ŭ���̹� �Լ�

	//���� ���� ������ ���������� ȣ��˴ϴ�.
	bool ClimbDownToFloor() const;

	bool CheckFloor(FHitResult& FloorHit) const;


	//Ŭ���̹� ����
	UFUNCTION(BlueprintCallable)
	bool TryClimbUpLedge(float ForwardValue);
	//������ ������ Top�κп� ��ֹ��� ������ Ȯ���մϴ�. ��ֹ��� ������ true�� ��ȯ�մϴ�.
	bool HasReachedEdge() const;
	//������ ���� �� �ִ��� Ȯ���մϴ�.
	bool IsLocationWalkable(const FVector& CheckLocation) const;
	//ĳ���Ͱ� �ö� ������ �ִ��� Ȯ���մϴ�.
	bool CanMoveToLedgeClimbLocation() const;
	//Ŭ���̹� ����

	//��ǲ Ŭ���̹�
	UFUNCTION(BlueprintCallable)
	bool CalculateJumpClimbPoint(float ForwardValue, float RightValue);
	UFUNCTION(BlueprintCallable)
	//��ġ, �����̼� ����
	void SetWarpJumpClimbValue(float ForwardValue, float RightValue);
	//��ǲ Ŭ���̹�


	//�������� üũ ����
	UPROPERTY(Category = "Climbing", EditAnywhere)
		int32 CollisionCapsuleRadius = 60;
	//�������� üũ ����
	UPROPERTY(Category = "Climbing", EditAnywhere)
		int32 CollisionCapsuleHalfHeight = 72;

	//���� Ž���� ��
	FHitResult CurrentWallHit;
	FHitResult CurrentLedgeHit;

	FHitResult CurrentJumpClimbHit;
	FHitResult CurrentJumpClimbWallHit;

	FCollisionQueryParams ClimbQueryparams;

	//���� �� �ִ� ���� ���� �����Ұ��̴�
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "75.0"))
		float MinHorizontalDegreesToStartClimbing = 25;

	bool bWantToClimb = false;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "72.0"))
		float ClimbingCollisionShrinkAmount = 30;

	//������ Ŭ���ֺ̹���
	FVector CurrentClimbingNormal;
	FVector CurrentClimbingPosition;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "500.0"))
		float MaxClimbingSpeed = 120.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "10.0", ClampMax = "2000.0"))
		float MaxClimbingAcceleration = 380.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "3000.0"))
		float BrakingDecelerationClimbing = 550.f;

	//Ŭ���̹� �����̼�
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "1.0", ClampMax = "12.0"))
		int ClimbingRotationSpeed = 6;

	//���� �����Ʈ
	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "60.0"))
		float ClimbingSnapSpeed = 4.f;

	UPROPERTY(Category = "Climbing", EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "80.0"))
		float DistanceFromSurface = 55.f;

	//1���ͰŸ����� �������� ���ڸ� ��������� ����մϴ�.
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

	//Ŭ���̹� ���� ��ǿ��� ����
	UPROPERTY(Category = "Climbing MotionWarping", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		bool IsJumpClimb;

	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FVector WarpJumpClimbingPoint;
	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FRotator WarpJumpClimbingRotation;

	UPROPERTY(Category = "Climbing MotionWarping", BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
		UAnimMontage* JumpClimbMontage;
};
