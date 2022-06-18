// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAssetManager.h"
#include "AbilitySystem/Items/ActionItem.h"
#include "AbilitySystemGlobals.h"

//const FPrimaryAssetType	UActionAssetManager::PotionItemType = TEXT("Potion");
//const FPrimaryAssetType	UActionAssetManager::SkillItemType = TEXT("Skill");
//const FPrimaryAssetType	UActionAssetManager::TokenItemType = TEXT("Token");
const FPrimaryAssetType	UActionAssetManager::WeaponItemType = TEXT("Weapon");

UActionAssetManager& UActionAssetManager::Get()
{
	UActionAssetManager* This = Cast<UActionAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogTemp, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be ActionAssetManager!"));
		return *NewObject<UActionAssetManager>(); // never calls this
	}
}

void UActionAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

UActionItem* UActionAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UActionItem* LoadedItem = Cast<UActionItem>(ItemPath.TryLoad());

	if (bLogWarning && LoadedItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to load item for identifier %s!"), *PrimaryAssetId.ToString());
	}

	return LoadedItem;
}
