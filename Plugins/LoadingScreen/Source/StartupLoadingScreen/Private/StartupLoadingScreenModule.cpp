// Copyright Epic Games, Inc. All Rights Reserved.

#include "StartupLoadingScreenModule.h"
#include "PreLoadScreenManager.h"
#include "StartupLoadingScreen.h"

#define LOCTEXT_NAMESPACE "FLoadingScreenModule"

void FStartupLoadingScreenModule::StartupModule()
{
	if (!IsRunningDedicatedServer())
	{
		PreLoadingScreen = MakeShared<FStartupLoadingScreen>();

		if (PreLoadingScreen.IsValid())
		{
			PreLoadingScreen->Init();

			if (FPreLoadScreenManager::Get())
			{
				FPreLoadScreenManager::Get()->RegisterPreLoadScreen(PreLoadingScreen);
				FPreLoadScreenManager::Get()->OnPreLoadScreenManagerCleanUp.AddRaw(
				this, &FStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp);
			}
		}
	}
}

void FStartupLoadingScreenModule::OnPreLoadScreenManagerCleanUp()
{
	if (PreLoadingScreen.IsValid())
	{
		ShutdownModule();
		PreLoadingScreen->CleanUp();
		//UE_LOG(LogTemp, Warning, TEXT("CleanUpStartupLoadingScreen"));
	}
	PreLoadingScreen.Reset();
}

void FStartupLoadingScreenModule::ShutdownModule()
{
	if (FPreLoadScreenManager::Get())
	{
		FPreLoadScreenManager::Get()->UnRegisterPreLoadScreen(PreLoadingScreen);
	}
}

bool FStartupLoadingScreenModule::IsGameModule() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FStartupLoadingScreenModule, StartupLoadingScreen)
