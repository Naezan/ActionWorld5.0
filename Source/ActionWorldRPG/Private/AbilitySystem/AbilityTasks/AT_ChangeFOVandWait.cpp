// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AT_ChangeFOVandWait.h"
#include "Camera/CameraComponent.h"
#include "Curves/CurveFloat.h"
#include "ActionWorldRPG/ActionWorldRPG.h"

UAT_ChangeFOVandWait::UAT_ChangeFOVandWait(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bTickingTask = true;
	bIsFinished = false;
}

UAT_ChangeFOVandWait* UAT_ChangeFOVandWait::WaitChangeFOV(UGameplayAbility* OwningAbility, FName TaskInstanceName, UCameraComponent* CameraComponent, float TargetFOV, float Duration, UCurveFloat* OptionalInterpolationCurve)
{
	UAT_ChangeFOVandWait* MyObj = NewAbilityTask<UAT_ChangeFOVandWait>(OwningAbility, TaskInstanceName);

	MyObj->CameraComponent = CameraComponent;
	if (CameraComponent != nullptr)
	{
		MyObj->StartFOV = MyObj->CameraComponent->FieldOfView;
	}

	MyObj->TargetFOV = TargetFOV;
	MyObj->Duration = FMath::Max(Duration, 0.001f);		// Avoid negative or divide-by-zero cases
	MyObj->TimeChangeStarted = MyObj->GetWorld()->GetTimeSeconds();
	MyObj->TimeChangeWillEnd = MyObj->TimeChangeStarted + MyObj->Duration;
	MyObj->LerpCurve = OptionalInterpolationCurve;

	return MyObj;
}

void UAT_ChangeFOVandWait::Activate()
{
}

void UAT_ChangeFOVandWait::TickTask(float DeltaTime)
{
	if (bIsFinished)
	{
		return;
	}

	Super::TickTask(DeltaTime);

	if (CameraComponent)
	{
		float CurrentTime = GetWorld()->GetTimeSeconds();

		//FOV 타임 끝
		if (CurrentTime >= TimeChangeWillEnd)
		{
			bIsFinished = true;

			CameraComponent->SetFieldOfView(TargetFOV);

			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnTargetFOVReached.Broadcast();
			}
			EndTask();
		}
		else
		{
			float NewFOV;

			//전체 시간중 얼마나 흘렀는지 비율
			float MoveFraction = (CurrentTime - TimeChangeStarted) / Duration;
			if (LerpCurve)
			{
				MoveFraction = LerpCurve->GetFloatValue(MoveFraction);
			}

			NewFOV = FMath::Lerp<float, float>(StartFOV, TargetFOV, MoveFraction);

			CameraComponent->SetFieldOfView(NewFOV);
		}
	}
	else
	{
		//카메라 없으면 종료
		bIsFinished = true;
		EndTask();
	}
}

void UAT_ChangeFOVandWait::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);
}
