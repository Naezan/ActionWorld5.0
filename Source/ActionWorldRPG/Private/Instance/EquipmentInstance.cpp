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
    //�÷��̾ Outer�� �� �� �ֵ��� ���������մϴ�.
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
        //������ �޽ø� ã���ϴ�.
        if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
        {
            AttachTarget = Char->GetMesh();
        }

        for (const FEquipmentSpawnInfo& SpawnInfo : ActorsToSpawn)
        {
            //���͸� �����մϴ�.
            AActor* NewActor = GetWorld()->SpawnActorDeferred<AActor>(SpawnInfo.ActorToSpawn, FTransform::Identity, OwningPawn);
            //������ �����մϴ�?
            NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
            //Ʈ�������� �����մϴ�.
            NewActor->SetActorRelativeTransform(SpawnInfo.AttachTransform);
            //���� ���Դϴ�.
            NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, SpawnInfo.AttachSocket);

            //������ ���͸� �����س����ϴ�.
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
