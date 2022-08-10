// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

class SStartupLoadingScreenWidget : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SStartupLoadingScreenWidget)
	{}
	SLATE_END_ARGS()
	void Construct(const FArguments& InArgs);
};
