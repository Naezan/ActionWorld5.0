// Fill out your copyright notice in the Description page of Project Settings.


#include "Instance/EquipmentDefinition.h"
#include "Instance/EquipmentInstance.h"


UEquipmentDefinition::UEquipmentDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstanceType = UEquipmentInstance::StaticClass();
}
