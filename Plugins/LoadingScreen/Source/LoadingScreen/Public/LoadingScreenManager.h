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
	//�ν��Ͻ��� ��������� ���� CDO���� ȣ��Ǵ� �Լ�
	//���������� �����ϰ� �ϱ� ���ؼ� ȣ���� �� �ִ�.
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//End Subsystem

	//�� �����Ӹ��� ȣ��Ǳ⶧���� ���ɻ� ���� ������
	//������ ���� ������Ʈ������ �� ����� �̿��ؼ� �ε� ��ũ���� �����ߴ�.
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

	//�ε� ��ũ���� ������ �ִ� ������Ʈ���� ȣ��
	void RegisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface);
	void UnregisterLoadingScreenInterface(TScriptInterface<ILoadingScreenInterface> Interface);

private:
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void HandlePostLoadMap(UWorld* World);

	//Calling Every Frame. Determine loading screen visibility
	void UpdateLoadingScreen();

	// �ε���ũ���� ǥ���ؾ߸� �ϴ� ��Ȳ�� üũ
	bool CheckForAnyNeedToShowLoadingScreen();

	// �ε���ũ���� �ʿ��� ��Ȳ�̰ų� ���������� �ε����ؾ��ϴ���
	bool ShouldShowLoadingScreen();

	// �ε���ũ���� �߱����� �ʱ��� �ε���ũ�� �÷ο쿡 �����ߴ���
	bool IsShowingInitialLoadingScreen() const;

	// �ε� ��ũ���� ���� �ε���ũ�� ���� ����
	void ShowLoadingScreen();

	// �ε� ��ũ���� ����� �ε���ũ�� ���� ����
	void HideLoadingScreen();


	/*
	* �ε��� ������
	*/

	// ����Ʈ���� ���� ����
	void RemoveWidgetFromViewport();

	// �ε� ��ũ���� �ߴ��� �Է� ����
	void StartBlockingInput();

	// �ε������� �Է� Ǯ��
	void StopBlockingInput();

	// PSOĳ�ø� �ε��մϴ�. ��ũ���� �ε�ɶ��� ������� �ѹ��� ȣ��˴ϴ�.
	void ChangePerformanceSettings(bool bEnableLoadingScreen);

private:
	//�ε���ũ���� �������ų� ������� ���ε��� �Լ� ��ε��ɽ���
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	//�ε� ��ũ���� ��� ����
	TSharedPtr<SWidget> LoadingScreenWidget;

	//�ε� ��ũ���� ���� ���� �Է��� ������ ������
	TSharedPtr<IInputProcessor> InputPreProcessor;

	//�ε� ��ũ�� �������̽��� ������ �ִ� ������Ʈ��
	TArray<TWeakInterfacePtr<ILoadingScreenInterface>> ExternalLoadingProcessors;

	//�ε� ��ũ�� ����� ����
	FString DebugLoadingScreenReason;

	//�ε� ��ũ���� �����ϴ� �ð�
	double LoadingScreenShownTime = 0.0;

	//�ε� ��ũ���� ���ַ����� �ð�
	double LoadingScreenDismissedTime = -1.0;

	//�ε� ��ũ���� ��� �����ִ� ���� ���� �αױ��� �ɸ��� �ð�
	double NextLogHeartBeatTime = 0.0;

	//�����ε�ʰ� ����Ʈ�ε�� �����϶� True
	bool bCurrentlyInLoadMap = false;

	//�ε� ��ũ���� �������� �ִ� ���̶�� True
	bool bCurrentlyShowingLoadingScreen = false;
};
