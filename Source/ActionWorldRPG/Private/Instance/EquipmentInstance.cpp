// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/EquipmentInstance.h"
#include "Instance/EquipmentDefinition.h"
#include "GameFramework/Character.h"

UEquipmentInstance::UEquipmentInstance(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
}

UWorld* UEquipmentInstance::GetWorld() const
{
    if (APawn* OwningPawn = GetPawn())
    {
        return OwningPawn->GetWorld();
    }

    return nullptr;
}

APawn* UEquipmentInstance::GetPawn() const
{
    //플레이어가 Outer가 될 수 있도록 만들어줘야합니다.
    return Cast<APawn>(GetOuter());
}

APawn* UEquipmentInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
    APawn* Result = nullptr;
    if (UClass* ActualPawnType = PawnType)
    {
        if (GetOuter()->IsA(ActualPawnType))
        {
            Result = Cast<APawn>(GetOuter());
        }
    }
    return Result;
}

void UEquipmentInstance::SpawnEquipmentActors(const TArray<FEquipmentSpawnInfo>& ActorsToSpawn)
{
    if (APawn* OwningPawn = GetPawn())
    {
        USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
        //장착할 메시를 찾습니다.
        if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
        {
            AttachTarget = Char->GetMesh();
        }

        for (const FEquipmentSpawnInfo& SpawnInfo : ActorsToSpawn)
        {
            //액터를 스폰합니다.
            AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
            //스폰을 종료합니다?
            NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
            //트랜스폼을 설정합니다.
            NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
            //본에 붙입니다.
            NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

            //스폰한 액터를 저장해놓습니다.
            SpawnedActors.Add(NewActor);
        }
    }
}

void UEquipmentInstance::DestroyEquipmentActors()
{
    for (AActor* Actor : SpawnedActors)
    {
        if (Actor)
        {
            Actor->Destroy();
        }
    }
}

void UEquipmentInstance::OnEquipped()
{
    K2_OnEquipped();
}

void UEquipmentInstance::OnUnequipped()
{
    K2_OnUnequipped();
}
