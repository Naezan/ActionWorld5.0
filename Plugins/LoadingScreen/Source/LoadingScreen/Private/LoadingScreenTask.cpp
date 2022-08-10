// Fill out your copyright notice in the Description page of Project Settings.


#include "LoadingScreenTask.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"
#include "LoadingScreenManager.h"


ULoadingScreenTask* ULoadingScreenTask::CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	ULoadingScreenManager* LoadingScreenManager = GameInstance ? GameInstance->GetSubsystem<ULoadingScreenManager>() : nullptr;

	if (LoadingScreenManager)
	{
		//로딩 테스크의 인스턴스는 로딩스크린 매니저가 소유한다
		//즉 로딩스크린태스크는 매니저에서만 생성되고 매니저에서 관리된다
		ULoadingScreenTask* NewLoadingTask = NewObject<ULoadingScreenTask>(LoadingScreenManager);
		NewLoadingTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);

		LoadingScreenManager->RegisterLoadingScreenInterface(NewLoadingTask);

		return NewLoadingTask;
	}

	return nullptr;
}

void ULoadingScreenTask::Unregister()
{
	//GetOuter는 여기선 this객체를 소유하고있는 UObject를 말합니다.
	ULoadingScreenManager* LoadingScreenManager = Cast<ULoadingScreenManager>(GetOuter());
	LoadingScreenManager->UnregisterLoadingScreenInterface(this);
}

void ULoadingScreenTask::SetShowLoadingScreenReason(const FString& InReason)
{
	Reason = InReason;
}

bool ULoadingScreenTask::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason = Reason;
	return true;
}
