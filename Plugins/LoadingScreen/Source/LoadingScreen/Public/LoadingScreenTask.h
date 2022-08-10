// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LoadingScreenInterface.h"

#include "LoadingScreenTask.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class LOADINGSCREEN_API ULoadingScreenTask : public UObject, public ILoadingScreenInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static ULoadingScreenTask* CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason);

public:
	ULoadingScreenTask() { }

	UFUNCTION(BlueprintCallable)
		void Unregister();

	UFUNCTION(BlueprintCallable)
		void SetShowLoadingScreenReason(const FString& InReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const override;

	FString Reason;
};
