// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "ActionAssetManager.generated.h"

class UActionItem;

/**
 * 
 */
UCLASS()
class ACTIONWORLDRPG_API UActionAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	// Constructor and overrides
	UActionAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Static types for items */
	//static const FPrimaryAssetType	PotionItemType;
	//static const FPrimaryAssetType	SkillItemType;
	static const FPrimaryAssetType	MiscItemType;
	static const FPrimaryAssetType	WeaponItemType;

	/** Returns the current AssetManager object */
	static UActionAssetManager& Get();
};
