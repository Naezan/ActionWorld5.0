// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Player/CustomCharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

UCustomCharacterMovementComponent::UCustomCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UCustomCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	AnimInstance = GetCharacterOwner()->GetMesh()->GetAnimInstance();

	ClimbQueryparams.AddIgnoredActor(GetOwner());
}

void UCustomCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//�� �Լ��� SpaceŰ�� �������� �����մϴ�.
	//SweepAndStoreWallHits();
}

void UCustomCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
	if (bWantToClimb)
	{
		SetMovementMode(EMovementMode::MOVE_Custom, ECustomMovementMode::CMOVE_Climbing);
	}

	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);
}

void UCustomCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	//���� Ŭ���̹� ���̶�� Ŭ���ֻ̹��°� ����� �ڿ� ȣ��˴ϴ�, Ŭ���ֻ̹��°� �ɶ� �ѹ� ȣ��˴ϴ�.
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
		Capsule->SetCapsuleRadius(Capsule->GetUnscaledCapsuleRadius() - 12);

		//�ӵ� �ʱ�ȭ �� �Լ��� �Ⱦ��ԵǸ� ��ģ���� �ӵ��� �ٸ��������� �̵��մϴ�
		StopMovementImmediately();

		//ĳ������ ��ġ�� ����ϴ�.
		FLatentActionInfo Info;
		Info.CallbackTarget = this;
		Info.ExecutionFunction = "OnEndClimbMove";
		Info.Linkage = 0;
		FRotator TragetRotation = UKismetMathLibrary::MakeRotFromX(-CurrentWallHit.Normal);
		UKismetSystemLibrary::MoveComponentTo(
			Capsule,
			CurrentLedgeHit.ImpactPoint - FVector(0, 0, 100) - UKismetMathLibrary::GetForwardVector(TragetRotation) * 33,
			TragetRotation,
			false,
			false,
			.2f,
			false,
			EMoveComponentAction::Type::Move,
			Info);
	}

	const bool bWasClimbing = PreviousMovementMode == MOVE_Custom && PreviousCustomMode == ECustomMovementMode::CMOVE_Climbing;
	//������ Ŭ���̹� ���̿��ٸ� Ŭ���̹� ���°� ����ɶ� �ѹ� ȣ��˴ϴ�
	if (bWasClimbing)
	{
		bIsPrevClambing = true;//�ӽÿ�
		bIsPlayClimbStart = true;//�ӽÿ�

		bOrientRotationToMovement = true;

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);
		Capsule->SetCapsuleRadius(Capsule->GetUnscaledCapsuleRadius() + 12);

		//�ӵ� �ʱ�ȭ
		StopMovementImmediately();
	}

	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);
}

void UCustomCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	if (CustomMovementMode == ECustomMovementMode::CMOVE_Climbing)
	{
		PhysClimbing(deltaTime, Iterations);
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UCustomCharacterMovementComponent::PhysClimbing(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	//ǥ�������� ���, �븻���Ϳ� ��ġ�� ���� ���
	ComputeSurfaceInfo();

	//Ŭ���̹� ���㿩��
	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	//Ŭ���̹� �ӵ� ���
	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	//�������� ���� ����
	MoveAlongClimbingSurface(deltaTime);

	//
	//TryClimbUpLedge();

	//��Ʈ����� ��������� �ʴٸ� ������ �ӵ����� �����մϴ�.
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	//ǥ�� Ÿ�� �����̱�?
	SnapToClimbingSurface(deltaTime);
}

float UCustomCharacterMovementComponent::GetMaxSpeed() const
{
	return IsClimbing() ? MaxClimbingSpeed : Super::GetMaxSpeed();
}

float UCustomCharacterMovementComponent::GetMaxAcceleration() const
{
	return IsClimbing() ? MaxClimbingAcceleration : Super::GetMaxAcceleration();
}

bool UCustomCharacterMovementComponent::IsCrouching() const
{
	return Super::IsCrouching();
}

bool UCustomCharacterMovementComponent::IsFalling() const
{
	return Super::IsFalling();
}

void UCustomCharacterMovementComponent::TryClimbing()
{
	if (CanStartClimbing())
	{
		bWantToClimb = true;
	}
}

void UCustomCharacterMovementComponent::CancelClimbing()
{
	bWantToClimb = false;
}

bool UCustomCharacterMovementComponent::IsClimbing() const
{
	return MovementMode == EMovementMode::MOVE_Custom && CustomMovementMode == ECustomMovementMode::CMOVE_Climbing;
}

FVector UCustomCharacterMovementComponent::GetClimbSurfaceNormal() const
{
	//ComputeSurfaceInfo�Լ��� ������ ȣ���ؾ߸��մϴ�.
	return CurrentClimbingNormal;
}

bool UCustomCharacterMovementComponent::SphereTraceAndStoreWallHit()
{
	for (int i = 0; i < 7; ++i)
	{
		const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20 + FVector(0, 0, 100 + 20 * i);//������ 20 ���� 100 + 20 * i

		const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
		const FVector End = Start + UpdatedComponent->GetForwardVector() * 100;//���� Offset�� �߰��ؼ� ������ ���� ����

		TArray<AActor*> IgnoreActor;
		FHitResult Result;
		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10, TraceTypeQuery1,
			false, IgnoreActor, EDrawDebugTrace::ForDuration, Result, true);

		//���� �ε����� ����Ʈ����Ʈ���� ���� �ѹ� �˻��մϴ�
		if (bIsHit)
		{
			const FVector NewStart = Result.ImpactPoint + FVector(0, 0, 20);
			const FVector NewEnd = NewStart + FVector(0, 0, -20);

			FHitResult NewResult;
			bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), NewStart, NewEnd, 10, TraceTypeQuery1,
				false, IgnoreActor, EDrawDebugTrace::ForDuration, NewResult, true);

			//�浹������ ���������� ���ο� �浹������ �����մϴ�.
			if (bIsNewHit)
			{
				if (!NewResult.bStartPenetrating)
				{
					CurrentWallHit = Result;
					CurrentWallHit.Normal = CurrentWallHit.Normal.GetSafeNormal2D();
					CurrentLedgeHit = NewResult;
					return true;
				}
			}
		}
	}

	//���������� �ݺ����� ���������� ���� Ž���߱⶧���� ���� �� �ִ� ���� �����ϴ�.
	CurrentWallHit.Reset();
	CurrentLedgeHit.Reset();
	return false;
}

bool UCustomCharacterMovementComponent::CanStartClimbing()
{
	if (CurrentWallHit.GetActor())
	{
		const FVector HorizontalNormal = CurrentWallHit.Normal.GetSafeNormal2D();//�����͸� �����ϰ� ����ȭ, ��鿡 ����

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(CurrentWallHit.Normal, HorizontalNormal);//������ ����

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);//0�̶�°� 90������ǹ̷� �븻���Ͱ� �Ʒ��� ���Ѵٴ� ���̴�.

		if (HorizontalDegrees <= MinHorizontalDegreesToStartClimbing &&
			!bIsCeiling &&
			IsFacingSurface(VerticalDot))
		{
			return true;
		}
	}

	return false;
}

bool UCustomCharacterMovementComponent::EyeHeightTrace(const float TraceDistance) const
{
	FHitResult UpperEdgeHit;

	//Ŭ���̹� ���̶�� ĸ���� EyeHeight�� �پ�� �����̱⿡ �پ�簪��ŭ ��������մϴ�.
	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + LedgeClimbEyeShrinkAmount : BaseEyeHeight;

	//�����̿��� TraceDistance��ŭ ������ �˻�
	const FVector Start = UpdatedComponent->GetComponentLocation() +
		(UpdatedComponent->GetUpVector() * EyeHeightOffset);
	const FVector End = Start + (UpdatedComponent->GetForwardVector() * TraceDistance);

	DrawDebugLine(
		GetWorld(),
		Start,
		End,
		FColor(255, 0, 0),
		true, -1, 0,
		2
	);
	return GetWorld()->LineTraceSingleByChannel(UpperEdgeHit, Start, End,
		ECC_WorldStatic, ClimbQueryparams);
}

bool UCustomCharacterMovementComponent::IsFacingSurface(const float Steepness) const
{
	constexpr float BaseLength = 80;
	const float SteepNessMultiplier = 1 + (1 - Steepness) * 5;

	return EyeHeightTrace(BaseLength * SteepNessMultiplier);
}

void UCustomCharacterMovementComponent::OnEndClimbMove()
{
	bIsPlayClimbStart = false;
}

void UCustomCharacterMovementComponent::ComputeSurfaceInfo()
{
	CurrentClimbingNormal = FVector::ZeroVector;
	CurrentClimbingPosition = FVector::ZeroVector;

	if (!CurrentWallHit.GetActor())
	{
		return;
	}

	//
	CurrentClimbingPosition = IsJumpClimb ? CurrentJumpClimbWallHit.ImpactPoint : CurrentWallHit.ImpactPoint;
	CurrentClimbingNormal = IsJumpClimb ? CurrentJumpClimbWallHit.Normal : CurrentWallHit.Normal;

	//��ġ�� ��հ�
	//CurrentClimbingPosition /= CurrentWallHits.Num();
	// 
	//�븻�� ���� ��պ���
	//CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UCustomCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	//��Ʈ��� �Լ��� ����Ǵ� ���� �������� ������ �ӵ����� �����մϴ�
	//�Ƹ� ��Ʈ����� �������� ���������� ���Ǵ� �ӵ������ϴ�
	RestorePreAdditiveRootMotionVelocity();

	//��Ʈ����� ��������� �ʴٸ� ������ �ӵ����� ����մϴ�
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	//��Ʈ����� �������� �ӵ��� �����մϴ�
	ApplyRootMotionToVelocity(deltaTime);
}

bool UCustomCharacterMovementComponent::ShouldStopClimbing()
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	//õ���̰ų� Ŭ���ֺ̹��͸�������� �ʾҰų� ���������°�쿡�� Ŭ���̹��� ���� ���մϴ�.
	//�߰��� �����߿��� Ŭ���̹��� ����ϴ�.
	return !bWantToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UCustomCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	bWantToClimb = false;
	//�������¸� �����մϴ�
	//IsLedgeClimbEnd = false;
	//���� Ŭ���̹� ���¸� �����մϴ�
	IsJumpClimb = false;
	//Ŭ���̹� ���ۺ����� �����մϴ�.
	bIsPlayClimbStart = true;

	//Ŭ���̹��� ����ϸ� �⺻������ Falling���°� �˴ϴ�.
	SetMovementMode(EMovementMode::MOVE_Falling);
	//�������Լ��� Falling���� ���Ӱ� ȣ���մϴ�.
	StartNewPhysics(deltaTime, Iterations);

}

void UCustomCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	//�������� �̿��ؼ� ��ü��ֹ��� ���� �����̼��� ������ �̵��ϴ�?
	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	//�̵��� �浹ü�� ������
	if (Hit.Time < 1.f)
	{
		//��ֹ� �浹�� ó���մϴ�
		HandleImpact(Hit, deltaTime, Adjusted);
		//��ü��ֹ��� ���鼭 �������ϴ�
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UCustomCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	//����Ʈ ����Ʈ���� ������ ��ġ�� ���� ���͸� ����� �� ���͸� ���麤�Ϳ� ������ŵ�ϴ�.
	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	//����Ʈ ����Ʈ���� �븻���͹������� ���� ������ ��ġ�� �������� ���մϴ�
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);
}

FQuat UCustomCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	//���� ȸ�����
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	//��Ʈ��� ���߿��� �Է±���� �ƴ϶� ��Ʈ��Ǳ������ ȸ���� �����մϴ�
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}

	//X�������ϴ� ȸ�����(������ ���������Ѵ�, Y��� Z�� ȸ������� �����Ѵ�)
	const FQuat Target = FRotationMatrix::MakeFromX(-CurrentClimbingNormal).ToQuat();

	return FMath::QInterpTo(Current, Target, deltaTime, ClimbingRotationSpeed);
}

bool UCustomCharacterMovementComponent::ClimbDownToFloor() const
{
	FHitResult FloorHit;
	if (!CheckFloor(FloorHit))
	{
		return false;
	}

	//������ ���� �� �ִ��� ���� GetWalkableFloorZ���� ������ ���� �� ���� �����̴�.
	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();

	//�ӵ��� Ŀ������ ������� ������ �������� ���ǵ�� �����ϴ�.
	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	//�ӵ��� ������ ũ�� ������ ���� �� �ִٸ�
	const bool bIsMovingTowardsFloor = DownSpeed >= MaxClimbingSpeed / 3 && bOnWalkableFloor;
	// ?
	const bool bIsClimbingFloor = CurrentClimbingNormal.Z > GetWalkableFloorZ();

	return bIsMovingTowardsFloor || (bIsClimbingFloor && bOnWalkableFloor);
}

bool UCustomCharacterMovementComponent::CheckFloor(FHitResult& FloorHit) const
{
	const FVector Start = UpdatedComponent->GetComponentLocation();
	const FVector End = Start + FVector::DownVector * FloorCheckDistance;


	return GetWorld()->LineTraceSingleByChannel(FloorHit, Start, End, ECC_WorldStatic, ClimbQueryparams);
}

bool UCustomCharacterMovementComponent::TryClimbUpLedge(float ForwardValue)
{
	if (AnimInstance && LedgeClimbMontage && AnimInstance->Montage_IsPlaying(LedgeClimbMontage))
	{
		return false;
	}

	//�Է°� 1���� üũ
	const bool bIsMovingUp = ForwardValue >= .9;

	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		//�¿���⸸ �����ϰ� �ִϸ��̼�������մϴ�
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		//AnimInstance->Montage_Play(LedgeClimbMontage);

		return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::HasReachedEdge() const
{
	//ĸ�� �������� 2.5�� �� ĸ���� ����� �� �ڸ��� �ִ��� Ȯ���մϴ�.
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 3.0f;

	return !EyeHeightTrace(TraceDistance);
}

bool UCustomCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	//üũ �Ÿ����� �Ʒ��� 250��ŭ�� �ִ� �ٴ��� üũ�մϴ�
	const FVector CheckEnd = CheckLocation + (FVector::DownVector * 250.f);

	DrawDebugLine(
		GetWorld(),
		CheckLocation,
		CheckEnd,
		FColor(255, 0, 0),
		true, -1, 0,
		2
	);

	FHitResult LedgeHit;
	const bool bHitLedgeGround = GetWorld()->LineTraceSingleByChannel(LedgeHit, CheckLocation, CheckEnd,
		ECC_WorldStatic, ClimbQueryparams);

	//�ٴ��� ������ ���� �� �ִ���
	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UCustomCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	//������120 ���� 160��ŭ�� �Ÿ����� �Ʒ��� üũ�մϴ�
	const FVector VerticalOffset = FVector::UpVector * 180.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 120.f;

	//ĸ������ ������120 ���� 160
	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	if (!IsLocationWalkable(CheckLocation))
	{
		return false;
	}

	//���� �� �ִٸ� ĳ������ �Ӹ��������� 160��ŭ�� ��ġ���� ������ ������ ������ üũ�մϴ�
	FHitResult CapsuleHit;
	const FVector CapsuleStartCheck = CheckLocation - HorizontalOffset;
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();

	//Debug
	/*DrawDebugCapsule(GetWorld(),
		CapsuleStartCheck,
		Capsule->GetUnscaledCapsuleHalfHeight(),
		Capsule->GetUnscaledCapsuleRadius(),
		FQuat::Identity,
		FColor::Cyan,
		true,
		-1,
		0,
		2
	);

	DrawDebugCapsule(GetWorld(),
		CheckLocation,
		Capsule->GetUnscaledCapsuleHalfHeight(),
		Capsule->GetUnscaledCapsuleRadius(),
		FQuat::Identity,
		FColor::Cyan,
		true,
		-1,
		0,
		2
	);*/

	const bool bBlocked = GetWorld()->SweepSingleByChannel(CapsuleHit, CapsuleStartCheck, CheckLocation,
		FQuat::Identity, ECC_WorldStatic, Capsule->GetCollisionShape(), ClimbQueryparams);

	return !bBlocked;
}

bool UCustomCharacterMovementComponent::CalculateJumpClimbPoint(float ForwardValue, float RightValue)
{
	//�Է°��� �޾� üũ�� ������ ���մϴ�.
	FVector Direction = FVector(0, RightValue, ForwardValue).GetSafeNormal();

	if (Direction.Size() == 0)
	{
		//���ڸ�
		if (AnimInstance && JumpClimbFailMontage && !AnimInstance->Montage_IsPlaying(JumpClimbFailMontage))
		{
			AnimInstance->Montage_Play(JumpClimbFailMontage);
		}
		return false;
	}

	for (int i = 0; i < 4; ++i)
	{
		const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20 + FVector(0, 0, 100) + Direction * (100 + 30 * i);/*FVector(0, 0, 130 + 30 * i);*/

		const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
		const FVector End = Start + UpdatedComponent->GetForwardVector() * 100;

		TArray<AActor*> IgnoreActor;
		FHitResult Result;
		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 30, TraceTypeQuery1,
			false, IgnoreActor, EDrawDebugTrace::ForDuration, Result, true);

		//���� Ž���ϸ� ���� �� �ִ� ��ġ�� Ž���մϴ�
		if (bIsHit)
		{
			FHitResult NewResult;
			bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Result.ImpactPoint + FVector(0, 0, 20), Result.ImpactPoint - FVector(0, 0, 5), 10, TraceTypeQuery1,
				false, IgnoreActor, EDrawDebugTrace::None, NewResult, true);

			//¤���� �ִ� ������ �ְ� �� ������ �븻���Ͱ� �����͸� ������ ���� 1�� �����ϸ� �� ���� �����Ѵٸ� ���� �� �ִ� �����̴�
			if (bIsNewHit)
			{
				bool bIsCatchable = FVector::DotProduct(NewResult.Normal, FVector::UpVector) > 0.9;
				if (bIsCatchable)
				{
					//���� ����
					CurrentJumpClimbHit = NewResult;
					CurrentJumpClimbWallHit = Result;
					//Z�� 1�ΰ�쿡�� �� ���Ϳ� ��
					if (CurrentJumpClimbWallHit.Normal.Z > 0.9)
						CurrentJumpClimbWallHit.Normal = FVector::CrossProduct(CurrentJumpClimbWallHit.Normal, UpdatedComponent->GetRightVector());
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%s"), *CurrentJumpClimbWallHit.Normal.ToString()));
					CurrentJumpClimbWallHit.Normal = CurrentJumpClimbWallHit.Normal.GetSafeNormal2D();
					return true;
				}
			}
		}
		//���� �ε����� ����Ʈ����Ʈ���� ���� �ѹ� �˻��մϴ�
		//if (bIsHit)
		//{
		//	const FVector NewStart = Result.ImpactPoint + Direction * 45;
		//	const FVector NewEnd = NewStart + Direction * -45;

		//	FHitResult NewResult;
		//	bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), NewStart, NewEnd, 15, TraceTypeQuery1,
		//		false, IgnoreActor, EDrawDebugTrace::ForDuration, NewResult, true);

		//	//�浹������ ���������� ���ο� �浹������ �����մϴ�.
		//	if (bIsNewHit)
		//	{
		//		
		//	}
		//}
	}

	CurrentJumpClimbHit.Reset();
	CurrentJumpClimbWallHit.Reset();
	return false;
}

void UCustomCharacterMovementComponent::SetWarpJumpClimbValue(float ForwardValue, float RightValue)
{
	//ĳ������ ��ġ�� ����ϴ�.
	//UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	//FLatentActionInfo Info;
	//Info.CallbackTarget = CharacterOwner;
	////Info.ExecutionFunction = "OnEndFunctionName";
	////Info.Linkage = 0;
	//FRotator TragetRotation = UKismetMathLibrary::MakeRotFromX(-CurrentJumpClimbWallHit.Normal);
	//UKismetSystemLibrary::MoveComponentTo(
	//	Capsule,
	//	CurrentJumpClimbHit.ImpactPoint - FVector(0, 0, 100) - UKismetMathLibrary::GetForwardVector(TragetRotation) * 33,
	//	TragetRotation,
	//	false,
	//	false,
	//	.2f,
	//	false,
	//	EMoveComponentAction::Type::Move,
	//	Info);

	JumpClimbMontage = ClimbJumpMontages[FMath::RoundToInt(-ForwardValue + 1) * 3 + FMath::RoundToInt(RightValue + 1)];

	FRotator TragetRotation = UKismetMathLibrary::MakeRotFromX(-CurrentJumpClimbWallHit.Normal);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("%s"), *CurrentJumpClimbWallHit.Normal.ToString()));
	if (ForwardValue != 0 || RightValue != 0)
		WarpJumpClimbingPoint = CurrentJumpClimbHit.ImpactPoint - FVector(0, 0, 162) - UKismetMathLibrary::GetForwardVector(TragetRotation) * 33;
	WarpJumpClimbingRotation = TragetRotation;
}