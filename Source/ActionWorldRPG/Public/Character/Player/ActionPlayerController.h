// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/Player/ModularPlayerController.h"
#include "AbilitySystem/ActionWorldAbilitySet.h"
#include "ActionPlayerController.generated.h"

class AActionCharacterBase;
class UInteractionHUD;
class UPaperSprite;

/**
 *
 */
UCLASS()
class ACTIONWORLDRPG_API AActionPlayerController : public AModularPlayerController
{
	GENERATED_BODY()

public:
	//Begin AActor
	virtual void PreInitializeComponents() override;
	// Server only
	virtual void OnRep_PlayerState() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//End AActor

	//Begin APlayerController
	virtual void ReceivedPlayer() override;
	//End APlayerController

public:
	void CreateInteractionHUD();

	UFUNCTION()
	UInteractionHUD* GetInteractionHUD();

	UFUNCTION()
		UUserWidget* GetBlackOutWidget();

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void SetEquippedWeaponPrimaryIconFromSprite(UPaperSprite* InSprite);

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void SetEquippedWeaponStatusText(const FText& StatusText);

	UFUNCTION(BlueprintPure, Category = "UI")
		int32 GetPrimaryClipAmmo();

	UFUNCTION(BlueprintPure, Category = "UI")
		int32 GetPrimaryReserveAmmo();

	UFUNCTION(BlueprintPure, Category = "UI")
		int32 GetSecondaryClipAmmo();

	UFUNCTION(BlueprintPure, Category = "UI")
		int32 GetSecondaryReserveAmmo();

	//UFUNCTION(BlueprintCallable, Category = "UI")
	//	void SetHUDReticle(TSubclassOf<class UGSHUDReticle> ReticleClass);

	UFUNCTION(Client, Reliable, WithValidation)
		void ShowDamageNumber(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	void ShowDamageNumber_Implementation(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);
	bool ShowDamageNumber_Validate(float DamageAmount, AActionCharacterBase* TargetCharacter, FGameplayTagContainer DamageNumberTags);

	// Simple way to RPC to the client the countdown until they respawn from the GameMode. Will be latency amount of out sync with the Server.
	UFUNCTION(Client, Reliable, WithValidation)
		void SetRespawnCountdown(float RespawnTimeRemaining);
	void SetRespawnCountdown_Implementation(float RespawnTimeRemaining);
	bool SetRespawnCountdown_Validate(float RespawnTimeRemaining);

	UFUNCTION(Client, Reliable, WithValidation)
		void ClientSetControlRotation(FRotator NewRotation);
	void ClientSetControlRotation_Implementation(FRotator NewRotation);
	bool ClientSetControlRotation_Validate(FRotator NewRotation);

protected:
	UFUNCTION(Exec)
		void Kill();

	UFUNCTION(Server, Reliable)
		void ServerKill();
	void ServerKill_Implementation();
	bool ServerKill_Validate();

protected:
	//플레이어 관련 위젯
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<UInteractionHUD> InteractionWidgetClass;

		//C++로 생성합니다 BP에서 생성할 수 없습니다
	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UInteractionHUD* InteractionWidget;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UI")
		TSubclassOf<UUserWidget> BlackOutWidgetClass;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
		UUserWidget* BlackOutWidget;

	//플레이어 컨트롤러는 스테이트정보를 기본적으로 가지고 있다

	//슬롯아이템 -> 현재 장착중인 아이템정보 여러가지일 수 있음 -> UI에 띄울수 있는 정보로 쓸수있을까?
};
