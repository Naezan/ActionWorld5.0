#include "StartupLoadingScreen.h"
#include "StartupLoadingScreenWidget.h"

void FStartupLoadingScreen::Init()
{
    FPreLoadScreenBase::Init();
    PreLoadingScreenWidget = SNew(SStartupLoadingScreenWidget);
    //UE_LOG(LogTemp, Warning, TEXT("StartUpLoadingScreenInit"));
}

void FStartupLoadingScreen::CleanUp()
{
    FPreLoadScreenBase::CleanUp();
    PreLoadingScreenWidget.Reset();
}
