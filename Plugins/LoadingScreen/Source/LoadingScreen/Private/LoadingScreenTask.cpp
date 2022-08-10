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
		//�ε� �׽�ũ�� �ν��Ͻ��� �ε���ũ�� �Ŵ����� �����Ѵ�
		//�� �ε���ũ���½�ũ�� �Ŵ��������� �����ǰ� �Ŵ������� �����ȴ�
		ULoadingScreenTask* NewLoadingTask = NewObject<ULoadingScreenTask>(LoadingScreenManager);
		NewLoadingTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);

		LoadingScreenManager->RegisterLoadingScreenInterface(NewLoadingTask);

		return NewLoadingTask;
	}

	return nullptr;
}

void ULoadingScreenTask::Unregister()
{
	//GetOuter�� ���⼱ this��ü�� �����ϰ��ִ� UObject�� ���մϴ�.
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
