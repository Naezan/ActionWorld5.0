// Fill out your copyright notice in the Description page of Project Settings.


#include "GameComponents/CombatComponent.h"

//캐릭터
#include "Character/Player/PlayerBase.h"

//무기
#include "Weapon/WeaponBase.h"

//컴포넌트
#include "Components/SphereComponent.h"

#include "Engine/SkeletalMeshSocket.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


//void UCombatComponent::BeginPlay()
//{
//	Super::BeginPlay();
//
//}
//
//
//// Called every frame
//void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
//{
//	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
//
//}
//
//void UCombatComponent::EquipWeapon(AWeaponBase* WeaponToEquip)
//{
//	if (Character == nullptr || WeaponToEquip == nullptr) return;
//
//	//EquippedWeapon = WeaponToEquip;
//	//EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
//
//	const USkeletalMeshSocket* HandSocket =
//		Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
//
//	if (HandSocket)
//	{
//		//HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
//	}
//
//	//EquippedWeapon->SetOwner(Character);
//}
//
//void UCombatComponent::SwapWeapons()
//{
//}
//
//void UCombatComponent::Reload()
//{
//}
//
//void UCombatComponent::FinishReloading()
//{
//}
//
//void UCombatComponent::FinishSwap()
//{
//}
//
//void UCombatComponent::FinishSwapAttachWeapons()
//{
//}
//
//void UCombatComponent::FireButtonPressed(bool bPressed)
//{
//}
//
