// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "PreLoadScreenBase.h"

class FStartupLoadingScreen : public FPreLoadScreenBase
{
public:
	FStartupLoadingScreen() {};
	virtual ~FStartupLoadingScreen() override {};

	virtual void Init() override;
	virtual void CleanUp() override;

	virtual TSharedPtr<SWidget> GetWidget() override { return PreLoadingScreenWidget; }
	virtual const TSharedPtr<const SWidget> GetWidget() const override { return PreLoadingScreenWidget; }
	virtual EPreLoadScreenTypes GetPreLoadScreenType() const override { return EPreLoadScreenTypes::EarlyStartupScreen; }

protected:
	TSharedPtr<SWidget> PreLoadingScreenWidget;
};
