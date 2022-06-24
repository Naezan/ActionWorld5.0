// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Monster/MonsterBase.h"

AMonsterBase::AMonsterBase()
{
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
}
