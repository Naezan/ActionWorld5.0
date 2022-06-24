// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage
{
	GENERATED_BODY()

public:
	TObjectPtr<class UTexture2D> CrosshairsCenter;
	TObjectPtr<class UTexture2D> CrosshairsLeft;
	TObjectPtr<class UTexture2D> CrosshairsRight;
	TObjectPtr<class UTexture2D> CrosshairsTop;
	TObjectPtr<class UTexture2D> CrosshairsBottom;
	float CrosshairSpread;
	FLinearColor CrosshairColor;
};

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

private:
	FHUDPackage HUDPackge;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, 
	FLinearColor CrosshairColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;
public:
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackge = Package; }
};
