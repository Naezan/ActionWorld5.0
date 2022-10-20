// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/EquipmentManagerComponent.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Instance/EquipmentInstance.h"
#include "Instance/EquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

#include "Weapon/WeaponBase.h"

//��񸮽�Ʈ
UEquipmentInstance* FEquipmentList::AddEquipmentItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	UEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	//�ӽ� ��ü�� ����ϴ�.
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	//����� �ν��Ͻ� ������ �����ɴϴ�.
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	//��񸮽�Ʈ�� ������� �Ҵ� �� ä���ֽ��ϴ�.
	FEquipmentItem& NewItem = EquipmentItems.AddDefaulted_GetRef();
	NewItem.EquipmentDefinition = EquipmentDefinition;
	NewItem.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewItem.Instance;

	//��� �ʿ��� ���͸� �������ݴϴ�.
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	//�����Ƽ�� �߰����ݴϴ�?
	for (const FEquipmentSpawnInfo& WeaponSpawnInfo : EquipmentCDO->ActorsToSpawn)
	{
		if (AWeaponBase* Weapon = Cast<AWeaponBase>(WeaponSpawnInfo.ActorToSpawn.GetDefaultObject()))
		{
			Weapon->AddWeaponInfoOnInteract(OwnerComponent->GetOwner());
		}
	}

	return Result;
}

void FEquipmentList::RemoveEquipmentItem(UEquipmentInstance* Instance)
{
	for (auto ItemIt = EquipmentItems.CreateIterator(); ItemIt; ++ItemIt)
	{
		FEquipmentItem& Item = *ItemIt;
		if (Item.Instance == Instance)
		{
			//��� �������ִ� ��� �����Ƽ�� �����մϴ�.
			for (const FEquipmentSpawnInfo& WeaponSpawnInfo : Item.EquipmentDefinition.GetDefaultObject()->ActorsToSpawn)
			{
				if (AWeaponBase* Weapon = Cast<AWeaponBase>(WeaponSpawnInfo.ActorToSpawn.GetDefaultObject()))
				{
					Weapon->RemoveAbilities();
				}
			}

			//��� �ʿ��� ���͸� �����մϴ�.
			Instance->DestroyEquipmentActors();

			//��񸮽�Ʈ���� ��� �����մϴ�.
			ItemIt.RemoveCurrent();
		}
	}
}

UActionAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UActionAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

//��� �Ŵ���
UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)//�����ϴ� ������Ʈ�� �ڱ� �ڽ��Դϴ�.
{
	bWantsInitializeComponent = true;
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

UEquipmentInstance* UEquipmentManagerComponent::EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	UEquipmentInstance* Result = nullptr;
	if (EquipmentDefinition != nullptr)
	{
		Result = EquipmentList.AddEquipmentItem(EquipmentDefinition);
		if (Result != nullptr)
		{
			//��� �����ϱ�
			Result->OnEquipped();
		}
	}

	return Result;
}

void UEquipmentManagerComponent::UnequipItem(UEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		//��� �����ϱ�
		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEquipmentItem(ItemInstance);
	}
}

void UEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UEquipmentInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FEquipmentItem& Entry : EquipmentList.EquipmentItems)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UEquipmentInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

UEquipmentInstance* UEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UEquipmentInstance> InstanceType)
{
	for (FEquipmentItem& Entry : EquipmentList.EquipmentItems)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UEquipmentInstance*> UEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UEquipmentInstance> InstanceType) const
{
	TArray<UEquipmentInstance*> Results;
	for (const FEquipmentItem& Entry : EquipmentList.EquipmentItems)
	{
		if (UEquipmentInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

