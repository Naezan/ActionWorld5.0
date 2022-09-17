// Fill out your copyright notice in the Description page of Project Settings.


#include "ActionAssetManager.h"
#include "AbilitySystem/Items/ActionItem.h"
#include "AbilitySystemGlobals.h"

//const FPrimaryAssetType	UActionAssetManager::PotionItemType = TEXT("Potion");
//const FPrimaryAssetType	UActionAssetManager::SkillItemType = TEXT("Skill");
const FPrimaryAssetType	UActionAssetManager::MiscItemType = TEXT("Miscellaneous");
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
