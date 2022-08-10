// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingScreenInterface.generated.h"

// 다른모듈에서 생성해서 사용하지 않을것이고 블루프린트에서 호출해서 사용가능하도록 한다
UINTERFACE(BlueprintType)
class ULoadingScreenInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LOADINGSCREEN_API ILoadingScreenInterface
{
	GENERATED_BODY()

public:
	static bool ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason);

	virtual bool ShouldShowLoadingScreen(FString& OutReason) const
	{
		return false;
	}
};
