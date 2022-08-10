// Fill out your copyright notice in the Description page of Project Settings.


#include "GameInstanceSubSystem/LoadingScreenSubsystem.h"

ULoadingScreenSubsystem::ULoadingScreenSubsystem()
{
}

void ULoadingScreenSubsystem::SetLoadingScreenContentWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
	if (LoadingScreenWidgetClass != NewWidgetClass)
	{
		LoadingScreenWidgetClass = NewWidgetClass;

		OnLoadingScreenWidgetChanged.Broadcast(LoadingScreenWidgetClass);
	}
}

TSubclassOf<UUserWidget> ULoadingScreenSubsystem::GetLoadingScreenContentWidget() const
{
	return LoadingScreenWidgetClass;
}
