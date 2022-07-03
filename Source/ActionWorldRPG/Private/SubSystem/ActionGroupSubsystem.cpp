// Fill out your copyright notice in the Description page of Project Settings.


#include "SubSystem/ActionGroupSubsystem.h"

//ÆÀ
#include "GenericTeamAgentInterface.h"

UActionGroupSubsystem::UActionGroupSubsystem()
{
}

void UActionGroupSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UActionGroupSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

bool UActionGroupSubsystem::ChangeGroupIDForActor(AActor* ActorToChange, int32 NewTeamId)
{
	const FGenericTeamId NewTeamID = (NewTeamId == INDEX_NONE) ? FGenericTeamId::NoTeam : FGenericTeamId((uint8)NewTeamId);
	if (IGenericTeamAgentInterface* TeamActor = Cast<IGenericTeamAgentInterface>(ActorToChange))
	{
		TeamActor->SetGenericTeamId(NewTeamID);
		return true;
	}
	else
	{
		return false;
	}
}

int32 UActionGroupSubsystem::FindGroupIDFromObject(const UObject* TestObject) const
{
	// See if it's directly a team agent
	if (const IGenericTeamAgentInterface* ObjectWithTeamInterface = Cast<IGenericTeamAgentInterface>(TestObject))
	{
		return (ObjectWithTeamInterface->GetGenericTeamId() == FGenericTeamId::NoTeam) ? 
		INDEX_NONE : (int32)ObjectWithTeamInterface->GetGenericTeamId();
	}

	if (const AActor* TestActor = Cast<const AActor>(TestObject))
	{
		// See if the instigator is a team actor
		if (const IGenericTeamAgentInterface* InstigatorWithTeamInterface = Cast<IGenericTeamAgentInterface>(TestActor->GetInstigator()))
		{
			return (InstigatorWithTeamInterface->GetGenericTeamId() == FGenericTeamId::NoTeam) ?
				INDEX_NONE : (int32)InstigatorWithTeamInterface->GetGenericTeamId();
		}
	}

	return INDEX_NONE;
}

EGroupComparison UActionGroupSubsystem::CompareGroup(const UObject* A, const UObject* B, int32& GroupIdA, int32& GroupIdB) const
{
	GroupIdA = FindGroupIDFromObject(Cast<const AActor>(A));
	GroupIdB = FindGroupIDFromObject(Cast<const AActor>(B));

	if (GroupIdA == INDEX_NONE || GroupIdB == INDEX_NONE)
	{
		return EGroupComparison::InvalidArgument;
	}
	else
	{
		return (GroupIdA == GroupIdB) ? EGroupComparison::OnSameGroup : EGroupComparison::DifferentGroup;
	}
}

EGroupComparison UActionGroupSubsystem::CompareGroup(const UObject* A, const UObject* B) const
{
	int32 GroupIdA;
	int32 GroupIdB;
	return CompareGroup(A, B, GroupIdA, GroupIdB);
}
