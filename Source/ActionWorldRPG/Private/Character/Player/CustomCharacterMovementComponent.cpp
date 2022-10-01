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

	//이 함수를 Space키가 눌렸을때 실행합니다.
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
	//현재 클라이밍 중이라면 클라이밍상태가 변경된 뒤에 호출됩니다, 클라이밍상태가 될때 한번 호출됩니다.
	if (IsClimbing())
	{
		bOrientRotationToMovement = false;

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() - ClimbingCollisionShrinkAmount);
		Capsule->SetCapsuleRadius(Capsule->GetUnscaledCapsuleRadius() - 12);

		//속도 초기화 이 함수를 안쓰게되면 미친듯한 속도로 다른방향으로 이동합니다
		StopMovementImmediately();

		//캐릭터의 위치를 맞춥니다.
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
	//이전에 클라이밍 중이였다면 클라이밍 상태가 변경될때 한번 호출됩니다
	if (bWasClimbing)
	{
		bIsPrevClambing = true;//임시용
		bIsPlayClimbStart = true;//임시용

		bOrientRotationToMovement = true;

		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
		Capsule->SetCapsuleHalfHeight(Capsule->GetUnscaledCapsuleHalfHeight() + ClimbingCollisionShrinkAmount);
		Capsule->SetCapsuleRadius(Capsule->GetUnscaledCapsuleRadius() + 12);

		//속도 초기화
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

	//표면정보를 계산, 노말벡터와 위치를 통해 계산
	ComputeSurfaceInfo();

	//클라이밍 멈춤여부
	if (ShouldStopClimbing() || ClimbDownToFloor())
	{
		StopClimbing(deltaTime, Iterations);
		return;
	}

	//클라이밍 속도 계산
	ComputeClimbingVelocity(deltaTime);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();

	//피직스를 통한 무브
	MoveAlongClimbingSurface(deltaTime);

	//
	//TryClimbUpLedge();

	//루트모션을 사용중이지 않다면 임의의 속도값을 지정합니다.
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / deltaTime;
	}

	//표면 타고 움직이기?
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
	//ComputeSurfaceInfo함수를 이전에 호출해야만합니다.
	return CurrentClimbingNormal;
}

bool UCustomCharacterMovementComponent::SphereTraceAndStoreWallHit()
{
	for (int i = 0; i < 7; ++i)
	{
		const FVector StartOffset = UpdatedComponent->GetForwardVector() * 20 + FVector(0, 0, 100 + 20 * i);//앞으로 20 위로 100 + 20 * i

		const FVector Start = UpdatedComponent->GetComponentLocation() + StartOffset;
		const FVector End = Start + UpdatedComponent->GetForwardVector() * 100;//작은 Offset을 추가해서 자잘한 버그 방지

		TArray<AActor*> IgnoreActor;
		FHitResult Result;
		bool bIsHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 10, TraceTypeQuery1,
			false, IgnoreActor, EDrawDebugTrace::ForDuration, Result, true);

		//벽과 부딪히면 임펙트포인트부터 위로 한번 검사합니다
		if (bIsHit)
		{
			const FVector NewStart = Result.ImpactPoint + FVector(0, 0, 20);
			const FVector NewEnd = NewStart + FVector(0, 0, -20);

			FHitResult NewResult;
			bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), NewStart, NewEnd, 10, TraceTypeQuery1,
				false, IgnoreActor, EDrawDebugTrace::ForDuration, NewResult, true);

			//충돌지점이 없을때까지 새로운 충돌지점을 갱신합니다.
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

	//정상적으로 반복문을 빠져나오면 벽만 탐지했기때문에 잡을 수 있는 곳이 없습니다.
	CurrentWallHit.Reset();
	CurrentLedgeHit.Reset();
	return false;
}

bool UCustomCharacterMovementComponent::CanStartClimbing()
{
	if (CurrentWallHit.GetActor())
	{
		const FVector HorizontalNormal = CurrentWallHit.Normal.GetSafeNormal2D();//업벡터를 제외하고 정규화, 평면에 투영

		const float HorizontalDot = FVector::DotProduct(UpdatedComponent->GetForwardVector(), -HorizontalNormal);
		const float VerticalDot = FVector::DotProduct(CurrentWallHit.Normal, HorizontalNormal);//땅과의 각도

		const float HorizontalDegrees = FMath::RadiansToDegrees(FMath::Acos(HorizontalDot));

		const bool bIsCeiling = FMath::IsNearlyZero(VerticalDot);//0이라는건 90도라는의미로 노말벡터가 아래를 향한다는 말이다.

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

	//클라이밍 중이라면 캡슐의 EyeHeight가 줄어든 상태이기에 줄어든값만큼 더해줘야합니다.
	const float BaseEyeHeight = GetCharacterOwner()->BaseEyeHeight;
	const float EyeHeightOffset = IsClimbing() ? BaseEyeHeight + LedgeClimbEyeShrinkAmount : BaseEyeHeight;

	//눈높이에서 TraceDistance만큼 앞으로 검사
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

	//위치의 평균값
	//CurrentClimbingPosition /= CurrentWallHits.Num();
	// 
	//노말의 정규 평균벡터
	//CurrentClimbingNormal = CurrentClimbingNormal.GetSafeNormal();
}

void UCustomCharacterMovementComponent::ComputeClimbingVelocity(float deltaTime)
{
	//루트모션 함수가 실행되는 동안 마지막에 더해진 속도값을 저장합니다
	//아마 루트모션이 끝났을때 임의적으로 사용되는 속도같습니다
	RestorePreAdditiveRootMotionVelocity();

	//루트모션이 사용중이지 않다면 임의의 속도값을 계산합니다
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		constexpr float Friction = 0.0f;
		constexpr bool bFluid = false;
		CalcVelocity(deltaTime, Friction, bFluid, BrakingDecelerationClimbing);
	}

	//루트모션의 변경점을 속도에 적용합니다
	ApplyRootMotionToVelocity(deltaTime);
}

bool UCustomCharacterMovementComponent::ShouldStopClimbing()
{
	const bool bIsOnCeiling = FVector::Parallel(CurrentClimbingNormal, FVector::UpVector);

	//천장이거나 클라이밍벡터를계산하지 않았거나 오를수없는경우에는 클라이밍을 하지 못합니다.
	//추가로 렛지중에는 클라이밍을 멈춥니다.
	return !bWantToClimb || CurrentClimbingNormal.IsZero() || bIsOnCeiling;
}

void UCustomCharacterMovementComponent::StopClimbing(float deltaTime, int32 Iterations)
{
	bWantToClimb = false;
	//렛지상태를 리셋합니다
	//IsLedgeClimbEnd = false;
	//점프 클라이밍 상태를 리셋합니다
	IsJumpClimb = false;
	//클라이밍 시작변수를 리셋합니다.
	bIsPlayClimbStart = true;

	//클라이밍을 취소하면 기본적으로 Falling상태가 됩니다.
	SetMovementMode(EMovementMode::MOVE_Falling);
	//피직스함수를 Falling으로 새롭게 호출합니다.
	StartNewPhysics(deltaTime, Iterations);

}

void UCustomCharacterMovementComponent::MoveAlongClimbingSurface(float deltaTime)
{
	const FVector Adjusted = Velocity * deltaTime;

	FHitResult Hit(1.f);

	//피직스를 이용해서 입체장애물을 피해 로테이션을 가지고 이동하는?
	SafeMoveUpdatedComponent(Adjusted, GetClimbingRotation(deltaTime), true, Hit);

	//이동중 충돌체가 있을때
	if (Hit.Time < 1.f)
	{
		//장애물 충돌을 처리합니다
		HandleImpact(Hit, deltaTime, Adjusted);
		//입체장애물을 비비면서 지나갑니다
		SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
	}
}

void UCustomCharacterMovementComponent::SnapToClimbingSurface(float deltaTime) const
{
	const FVector Forward = UpdatedComponent->GetForwardVector();
	const FVector Location = UpdatedComponent->GetComponentLocation();
	const FQuat Rotation = UpdatedComponent->GetComponentQuat();

	//임펙트 포인트에서 현재의 위치를 빼준 벡터를 만들고 그 벡터를 정면벡터에 투영시킵니다.
	const FVector ForwardDifference = (CurrentClimbingPosition - Location).ProjectOnTo(Forward);
	//임펙트 포인트에서 노말벡터방향으로 조금 떨어진 위치를 기준으로 정합니다
	const FVector Offset = -CurrentClimbingNormal * (ForwardDifference.Length() - DistanceFromSurface);
}

FQuat UCustomCharacterMovementComponent::GetClimbingRotation(float deltaTime) const
{
	//현재 회전행렬
	const FQuat Current = UpdatedComponent->GetComponentQuat();

	//루트모션 도중에는 입력기반이 아니라 루트모션기반으로 회전을 적용합니다
	if (HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity())
	{
		return Current;
	}

	//X축으로하는 회전행렬(정면을 기준으로한다, Y축과 Z축 회전행렬은 무시한다)
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

	//지면이 걸을 수 있는지 여부 GetWalkableFloorZ보다 작으면 걸을 수 없는 지면이다.
	const bool bOnWalkableFloor = FloorHit.Normal.Z > GetWalkableFloorZ();

	//속도가 커질수록 지면과의 각도가 작을수록 스피드는 빠릅니다.
	const float DownSpeed = FVector::DotProduct(Velocity, -FloorHit.Normal);
	//속도가 일정량 크고 지면이 걸을 수 있다면
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

	//입력값 1인지 체크
	const bool bIsMovingUp = ForwardValue >= .9;

	if (bIsMovingUp && HasReachedEdge() && CanMoveToLedgeClimbLocation())
	{
		//좌우방향만 결정하고 애니메이션을재생합니다
		const FRotator StandRotation = FRotator(0, UpdatedComponent->GetComponentRotation().Yaw, 0);
		UpdatedComponent->SetRelativeRotation(StandRotation);

		//AnimInstance->Montage_Play(LedgeClimbMontage);

		return true;
	}

	return false;
}

bool UCustomCharacterMovementComponent::HasReachedEdge() const
{
	//캡슐 반지름의 2.5배 즉 캡슐이 충분히 들어갈 자리가 있는지 확인합니다.
	const UCapsuleComponent* Capsule = CharacterOwner->GetCapsuleComponent();
	const float TraceDistance = Capsule->GetUnscaledCapsuleRadius() * 3.0f;

	return !EyeHeightTrace(TraceDistance);
}

bool UCustomCharacterMovementComponent::IsLocationWalkable(const FVector& CheckLocation) const
{
	//체크 거리에서 아래러 250만큼에 있는 바닥을 체크합니다
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

	//바닥의 각도가 걸을 수 있는지
	return bHitLedgeGround && LedgeHit.Normal.Z >= GetWalkableFloorZ();
}

bool UCustomCharacterMovementComponent::CanMoveToLedgeClimbLocation() const
{
	//앞으로120 위로 160만큼의 거리에서 아래로 체크합니다
	const FVector VerticalOffset = FVector::UpVector * 180.f;
	const FVector HorizontalOffset = UpdatedComponent->GetForwardVector() * 120.f;

	//캡슐에서 앞으로120 위로 160
	const FVector CheckLocation = UpdatedComponent->GetComponentLocation() + HorizontalOffset + VerticalOffset;

	if (!IsLocationWalkable(CheckLocation))
	{
		return false;
	}

	//걸을 수 있다면 캐릭터의 머리지점에서 160만큼의 위치까지 막히는 구간이 없는지 체크합니다
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
	//입력값을 받아 체크할 방향을 구합니다.
	FVector Direction = FVector(0, RightValue, ForwardValue).GetSafeNormal();

	if (Direction.Size() == 0)
	{
		//제자리
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

		//벽을 탐지하면 잡을 수 있는 위치를 탐색합니다
		if (bIsHit)
		{
			FHitResult NewResult;
			bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Result.ImpactPoint + FVector(0, 0, 20), Result.ImpactPoint - FVector(0, 0, 5), 10, TraceTypeQuery1,
				false, IgnoreActor, EDrawDebugTrace::None, NewResult, true);

			//짚을수 있는 지점이 있고 그 지점의 노말벡터가 업벡터를 내적한 값이 1에 근접하면 즉 거의 평행한다면 잡을 수 있는 지점이다
			if (bIsNewHit)
			{
				bool bIsCatchable = FVector::DotProduct(NewResult.Normal, FVector::UpVector) > 0.9;
				if (bIsCatchable)
				{
					//정보 저장
					CurrentJumpClimbHit = NewResult;
					CurrentJumpClimbWallHit = Result;
					//Z가 1인경우에는 이 벡터와 적
					if (CurrentJumpClimbWallHit.Normal.Z > 0.9)
						CurrentJumpClimbWallHit.Normal = FVector::CrossProduct(CurrentJumpClimbWallHit.Normal, UpdatedComponent->GetRightVector());
					GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, FString::Printf(TEXT("%s"), *CurrentJumpClimbWallHit.Normal.ToString()));
					CurrentJumpClimbWallHit.Normal = CurrentJumpClimbWallHit.Normal.GetSafeNormal2D();
					return true;
				}
			}
		}
		//벽과 부딪히면 임펙트포인트부터 위로 한번 검사합니다
		//if (bIsHit)
		//{
		//	const FVector NewStart = Result.ImpactPoint + Direction * 45;
		//	const FVector NewEnd = NewStart + Direction * -45;

		//	FHitResult NewResult;
		//	bool bIsNewHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), NewStart, NewEnd, 15, TraceTypeQuery1,
		//		false, IgnoreActor, EDrawDebugTrace::ForDuration, NewResult, true);

		//	//충돌지점이 없을때까지 새로운 충돌지점을 갱신합니다.
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
	//캐릭터의 위치를 맞춥니다.
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