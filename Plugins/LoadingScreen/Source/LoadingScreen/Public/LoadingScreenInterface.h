// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LoadingScreenInterface.generated.h"

// �ٸ���⿡�� �����ؼ� ������� �������̰� �������Ʈ���� ȣ���ؼ� ��밡���ϵ��� �Ѵ�
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
