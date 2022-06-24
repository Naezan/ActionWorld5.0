// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/ActionCharacterBase.h"

//인터페이스
#include "Interfaces/InteractCrosshairInterface.h"

#include "MonsterBase.generated.h"

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API AMonsterBase : public AActionCharacterBase, public IInteractCrosshairInterface
{
	GENERATED_BODY()

public:
	AMonsterBase();
};
