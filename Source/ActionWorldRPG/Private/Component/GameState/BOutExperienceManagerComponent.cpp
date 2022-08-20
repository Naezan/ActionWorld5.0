// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/GameState/BOutExperienceManagerComponent.h"
#include "GameFeaturesSubsystem.h"

void UBOutExperienceManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UBOutExperienceManagerComponent::LoadAndActivateGameFeaturePlugin(FString GameFeaturePluginName, bool bEnable)
{
	FString PluginURL;

	UGameFeaturesSubsystem::Get().GetPluginURLForBuiltInPluginByName(GameFeaturePluginName, PluginURL);
	bEnable ? UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete()) : UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL);
	// Or UGameFeaturesSubsystem::Get().UnloadGameFeaturePlugin(PluginURL); 하지만 이함수를 쓰게되면 콘텐츠 브라우저에서도 안보인다.
}