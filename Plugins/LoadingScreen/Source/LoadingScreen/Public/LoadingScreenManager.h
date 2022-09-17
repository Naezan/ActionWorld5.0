// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/WeakInterfacePtr.h"

#include "Tickable.h"

#include "LoadingScreenManager.generated.h"

class SWidget;
class IInputProcessor;
class ILoadingScreenInterface;
struct FWorldContext;

UCLASS()
class LOADINGSCREEN_API ULoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	//Begin Subsystem
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//인스턴스가 만들어지기 전에 CDO에서 호출되는 함수
	//서버에서만 동작하게 하기 위해서 호출할 수 있다.
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//End Subsystem

	//매 프레임마다 호출되기때문에 성능상 좋진 않지만
	//왜인지 샘플 프로젝트에서는 이 기능을 이용해서 로딩 스크린을 구현했다.
	//Begin Tickable
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	//End Tickable

	UFUNCTION(BlueprintCallable, Category = LoadingScreen)
		FString GetDebugLoadingScreenReason() const
	{
		return DebugLoadingScreenReason;
	}

	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}

	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);

	FORCEINLINE FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChangedDelegate()
	{
		return LoadingScreenVisibilityChanged;
	}

	//로딩 스크린을 가지고 있는 오브젝트에서 호출
	void RegisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface);
	void UnregisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface);

private:
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void HandlePostLoadMap(UWorld* World);

	//Calling Every Frame. Determine loading screen visibility
	void UpdateLoadingScreen();

	// 로딩스크린을 표시해야만 하는 상황을 체크
	bool CheckForAnyNeedToShowLoadingScreen();

	// 로딩스크린이 필요한 상황이거나 강제적으로 로딩을해야하는지
	bool ShouldShowLoadingScreen();

	// 로딩스크린이 뜨기전에 초기의 로딩스크린 플로우에 진입했는지
	bool IsShowingInitialLoadingScreen() const;

	// 로딩 스크린을 띄우고 로딩스크린 위젯 셋팅
	void ShowLoadingScreen();

	// 로딩 스크린을 숨기고 로딩스크린 위젯 삭제
	void HideLoadingScreen();


	/*
	* 로딩이 끝난후
	*/

	// 뷰포트에서 위젯 제거
	void RemoveWidgetFromViewport();

	// 로딩 스크린이 뜨는중 입력 막기
	void StartBlockingInput();

	// 로딩끝난후 입력 풀기
	void StopBlockingInput();

	// PSO캐시를 로드합니다. 스크린이 로드될때와 사라질때 한번씩 호출됩니다.
	void ChangePerformanceSettings(bool bEnableLoadingScreen);

private:
	//로딩스크린이 보여지거나 사라질때 바인딩된 함수 브로드케스팅
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	//로딩 스크린에 띄울 위젯
	TSharedPtr<SWidget> LoadingScreenWidget;

	//로딩 스크린을 띄우는 동안 입력을 제어할 관리자
	TSharedPtr<IInputProcessor> InputPreProcessor;

	//로딩 스크린 인터페이스를 가지고 있는 오브젝트들
	TArray<TWeakInterfacePtr<ILoadingScreenInterface>> ExternalLoadingProcessors;

	//로딩 스크린 디버그 내용
	FString DebugLoadingScreenReason;

	//로딩 스크린이 시작하는 시간
	double LoadingScreenShownTime = 0.0;

	//로딩 스크린을 없애려고한 시간
	double LoadingScreenDismissedTime = -1.0;

	//로딩 스크린이 계속 켜져있는 동안 다음 로그까지 걸리는 시간
	double NextLogHeartBeatTime = 0.0;

	//프리로드맵과 포스트로드맵 사이일때 True
	bool bCurrentlyInLoadMap = false;

	//로딩 스크린이 보여지고 있는 중이라면 True
	bool bCurrentlyShowingLoadingScreen = false;
};
