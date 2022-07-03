// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "ActionGroupSubsystem.generated.h"

// Result of comparing the team affiliation for two actors
UENUM(BlueprintType)
enum class EGroupComparison : uint8
{
	// Both actors are members of the same team
	OnSameGroup,

	// The actors are members of opposing teams
	DifferentGroup,

	// One (or both) of the actors was invalid or not part of any team
	InvalidArgument
};

UCLASS()
class ACTIONWORLDRPG_API UActionGroupSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UActionGroupSubsystem();

	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	//~End of USubsystem interface

	// Changes the team associated with this actor if possible
	UFUNCTION(BlueprintCallable, Category = Group)
	bool ChangeGroupIDForActor(AActor* ActorToChange, int32 NewTeamId);

	// Returns the team this object belongs to, or INDEX_NONE if it is not part of a team
	int32 FindGroupIDFromObject(const UObject* TestObject) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = Group, meta = (ExpandEnumAsExecs = ReturnValue))
	EGroupComparison CompareGroup(const UObject* A, const UObject* B, int32& GroupIdA, int32& GroupIdB) const;

	EGroupComparison CompareGroup(const UObject* A, const UObject* B) const;
};
