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

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Only called on the Server. Calls before Server's AcknowledgePossession.
	virtual void PossessedBy(AController* NewController) override;

protected:
	UPROPERTY()
	class AMonsterAIController* MonsterController;

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

protected:

	void RelativeVectorToWorldVector(TArray<FVector>& VectorArray);
};
