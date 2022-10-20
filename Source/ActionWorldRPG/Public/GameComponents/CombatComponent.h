// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONWORLDRPG_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
//	friend class APlayerBase;//플레이어 클래스에서 자유롭게접근 가능
//	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//
//	void EquipWeapon(class AWeaponBase* WeaponToEquip);
//	void SwapWeapons();
//	void Reload();
//	UFUNCTION(BlueprintCallable)
//		void FinishReloading();
//
//	UFUNCTION(BlueprintCallable)
//		void FinishSwap();
//
//	UFUNCTION(BlueprintCallable)
//		void FinishSwapAttachWeapons();
//
//	void FireButtonPressed(bool bPressed);
//
//protected:
//	virtual void BeginPlay() override;
//
//private:
//	UPROPERTY()
//		class APlayerBase* Character;
//	UPROPERTY()
//		class AActionPlayerController* Controller;
//
//	UPROPERTY()
//		AWeaponBase* EquippedWeapon;
//
//	UPROPERTY()
//		bool bAiming;
};
