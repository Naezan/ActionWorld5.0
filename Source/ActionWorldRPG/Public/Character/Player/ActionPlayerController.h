// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AbilitySystem/ActionWorldAbilitySet.h"
#include "ActionPlayerController.generated.h"

class AActionCharacterBase;
class UPaperSprite;

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API AActionPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetEquippedWeaponStatusText(const FText& StatusText);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetPrimaryClipAmmo(int32 ClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetPrimaryReserveAmmo(int32 ReserveAmmo);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetSecondaryClipAmmo(int32 SecondaryClipAmmo);

	UFUNCTION(BlueprintCallable, Category = "UI")
		void SetSecondaryReserveAmmo(int32 SecondaryReserveAmmo);

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void SetHUDReticle(TSubclassOf<class UGSHUDReticle> ReticleClass);

	UFUNCTION(Client, Reliable, WithValidation)
	void ShowDamageNumber(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	void ShowDamageNumber_Implementation(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	bool ShowDamageNumber_Validate(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);


	//플레이어 관련 위젯

	//플레이어 컨트롤러는 스테이트정보를 기본적으로 가지고 있다

	//슬롯아이템 -> 현재 장착중인 아이템정보 여러가지일 수 있음 -> UI에 띄울수 있는 정보로 쓸수있을까?
};
