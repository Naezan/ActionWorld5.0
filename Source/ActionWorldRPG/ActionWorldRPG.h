// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define EPS_Grass	EPhysicalSurface::SurfaceType1
#define EPS_Stone	EPhysicalSurface::SurfaceType2
#define EPS_Metal	EPhysicalSurface::SurfaceType3
#define EPS_Wood	EPhysicalSurface::SurfaceType4
#define EPS_Sand	EPhysicalSurface::SurfaceType5
#define EPS_Water	EPhysicalSurface::SurfaceType6

UENUM(BlueprintType)
enum class EActionAbilityInputID : uint8
{
	// 0 None
	None				UMETA(DisplayName = "None"),
	// 1 Confirm
	Confirm				UMETA(DisplayName = "Confirm"),
	// 2 Cancel
	Cancel				UMETA(DisplayName = "Cancel"),
	// 3 Sprint
	Sprint				UMETA(DisplayName = "Sprint"),
	// 4 Jump
	Jump				UMETA(DisplayName = "Jump"),
	// 5 PrimaryFire
	PrimaryFire			UMETA(DisplayName = "Primary Fire"),
	// 6 SecondaryFire
	SecondaryFire		UMETA(DisplayName = "Secondary Fire"),
	// 7 Alternate Fire
	AlternateFire		UMETA(DisplayName = "Alternate Fire"),
	// 8 Reload
	Reload				UMETA(DisplayName = "Reload"),
	// 9 NextWeapon
	NextWeapon			UMETA(DisplayName = "Next Weapon"),
	// 10 PrevWeapon
	PrevWeapon			UMETA(DisplayName = "Previous Weapon"),
	// 11 Interact
	Interact			UMETA(DisplayName = "Interact"),
	// 12 Emote
	Emote				UMETA(DisplayName = "Emote"),
};
