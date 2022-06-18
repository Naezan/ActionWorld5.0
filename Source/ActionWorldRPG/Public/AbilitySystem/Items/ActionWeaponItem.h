#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Items/ActionItem.h"
#include "ActionWeaponItem.generated.h"

/** Native base class for weapons, should be blueprinted */
UCLASS()
class ACTIONWORLDRPG_API UActionWeaponItem : public UActionItem
{
	GENERATED_BODY()

public:
	/** Constructor */
	UActionWeaponItem()
	{
		ItemType = UActionAssetManager::WeaponItemType;
	}

	/** Weapon actor to spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Weapon)
		TSubclassOf<AActor> WeaponActor;
};