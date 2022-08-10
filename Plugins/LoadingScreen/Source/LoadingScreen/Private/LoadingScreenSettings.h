// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "LoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Loading Screen"))
class ULoadingScreenSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	ULoadingScreenSettings();

public:

	UPROPERTY(config, EditAnywhere, Category = Display, meta = (MetaClass = "UserWidget"))
		FSoftClassPath LoadingScreenWidget;

	UPROPERTY(config, EditAnywhere, Category = Display)
		int32 LoadingScreenZOrder = 10000;

	// How long to hold the loading screen up after other loading finishes (in seconds) to
	// try to give texture streaming a chance to avoid blurriness
	//
	// Note: This is not normally applied in the editor for iteration time, but can be 
	// enabled via HoldLoadingScreenAdditionalSecsEvenInEditor
	UPROPERTY(config, EditAnywhere, Category = Configuration, meta = (ForceUnits = s))
		float HoldLoadingScreenAdditionalSecs = 2.0f;

	// The interval in seconds beyond which the loading screen is considered permanently hung (if non-zero).
	UPROPERTY(config, EditAnywhere, Category = Configuration, meta = (ForceUnits = s))
		float LoadingScreenHeartbeatHangDuration = 0.0f;

	// 로딩 스크린을 띄우는 주체가 뭔지에 대한 로그의 간격.
	UPROPERTY(config, EditAnywhere, Category = Configuration, meta = (ForceUnits = s))
		float LogLoadingScreenHeartbeatInterval = 5.0f;

	// When true, the reason the loading screen is shown or hidden will be printed to the log every frame.
	UPROPERTY(Transient, EditAnywhere, Category = Debugging)
		bool LogLoadingScreenReasonEveryFrame = 0;

	// Force the loading screen to be displayed (useful for debugging)
	UPROPERTY(Transient, EditAnywhere, Category = Debugging)
		bool ForceLoadingScreenVisible = false;

	// 에디터에서 추가 지연 시간을 추가하는지 여부
	// (useful when iterating on loading screens)
	UPROPERTY(Transient, EditAnywhere, Category = Debugging)
		bool HoldLoadingScreenAdditionalSecsEvenInEditor = false;

	// Should we apply the additional HoldLoadingScreenAdditionalSecs delay even in the editor
	// (useful when iterating on loading screens)
	UPROPERTY(config, EditAnywhere, Category = Configuration)
		bool ForceTickLoadingScreenEvenInEditor = true;
};
