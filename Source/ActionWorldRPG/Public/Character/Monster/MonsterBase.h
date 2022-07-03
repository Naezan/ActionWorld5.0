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

	// Friended to allow access to handle functions above
	friend UMonsterAttributeSet;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

	//AttributeSet
	class UMonsterAttributeSet* GetMonsterAttributeSet() const;

protected:
	class AMonsterAIController* MonsterController;

	/** List of attributes modified by the ability system */
	UPROPERTY()
		UMonsterAttributeSet* MonsterAttributeSet;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Monster")
		TSubclassOf<UGameplayEffect> DeathEffect;

	/* Point for Monster to move to */
	UPROPERTY(EditAnywhere, Category = "AI", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
		TArray<FVector> PatrolPoint;

private:
	bool bHelped = false;

public:
	UFUNCTION(BlueprintCallable, Category = "AI")
		FORCEINLINE	TArray<FVector> GetPatrolPoint() const { return PatrolPoint; }

	UFUNCTION(BlueprintCallable, Category = "AI")
		FORCEINLINE	bool GetHelped() const { return bHelped; }

	//===========================
	// Get Attributes
	//===========================
	virtual int32 GetCharacterLevel() const override;

	virtual float GetHealth() const override;

	virtual float GetMaxHealth() const override;

	virtual float GetMana() const override;

	virtual float GetMaxMana() const override;

	virtual float GetStamina() const override;

	virtual float GetMaxStamina() const override;

	virtual float GetShield() const override;

	virtual float GetMaxShield() const override;

	// Gets the Current value of MoveSpeed
	virtual float GetMoveSpeed() const override;

	// Gets the Base value of MoveSpeed
	virtual float GetMoveSpeedBaseValue() const override;

protected:

	void RelativeVectorToWorldVector(TArray<FVector>& VectorArray);
	/*
	* AttributeSet
	*/
	virtual void SetHealth(float Health) override;
	virtual void SetMana(float Mana) override;
	virtual void SetStamina(float Stamina) override;
	virtual void SetShield(float Shield) override;
};
