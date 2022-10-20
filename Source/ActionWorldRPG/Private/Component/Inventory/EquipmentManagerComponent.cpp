// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/EquipmentManagerComponent.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Instance/EquipmentInstance.h"
#include "Instance/EquipmentDefinition.h"
#include "Net/UnrealNetwork.h"

#include "Weapon/WeaponBase.h"

//장비리스트
UEquipmentInstance* FEquipmentList::AddEquipmentItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition)
{
	UEquipmentInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());

	//임시 객체를 만듭니다.
	const UEquipmentDefinition* EquipmentCDO = GetDefault<UEquipmentDefinition>(EquipmentDefinition);

	//장비의 인스턴스 정보를 가져옵니다.
	TSubclassOf<UEquipmentInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UEquipmentInstance::StaticClass();
	}

	//장비리스트에 빈공간을 할당 및 채워넣습니다.
	FEquipmentItem& NewItem = EquipmentItems.AddDefaulted_GetRef();
	NewItem.EquipmentDefinition = EquipmentDefinition;
	NewItem.Instance = NewObject<UEquipmentInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewItem.Instance;

	//장비에 필요한 액터를 스폰해줍니다.
	Result->SpawnEquipmentActors(EquipmentCDO->ActorsToSpawn);

	//어빌리티를 추가해줍니다?
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
			//장비가 가지고있는 모든 어빌리티를 제거합니다.
			for (const FEquipmentSpawnInfo& WeaponSpawnInfo : Item.EquipmentDefinition.GetDefaultObject()->ActorsToSpawn)
			{
				if (AWeaponBase* Weapon = Cast<AWeaponBase>(WeaponSpawnInfo.ActorToSpawn.GetDefaultObject()))
				{
					Weapon->RemoveAbilities();
				}
			}

			//장비에 필요한 액터를 삭제합니다.
			Instance->DestroyEquipmentActors();

			//장비리스트에서 장비를 제거합니다.
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

//장비 매니저
UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)//소유하는 컴포넌트가 자기 자신입니다.
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
			//장비 장착하기
			Result->OnEquipped();
		}
	}

	return Result;
}

void UEquipmentManagerComponent::UnequipItem(UEquipmentInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		//장비 해제하기
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

