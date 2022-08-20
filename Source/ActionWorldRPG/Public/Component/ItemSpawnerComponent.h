// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemSpawnerComponent.generated.h"

//�������� ������ ��ġ�� �� �ֵ��� �����ִ� ������Ʈ�Դϴ�.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONWORLDRPG_API UItemSpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemSpawnerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	UFUNCTION(BlueprintCallable)
	bool SpawnItem(/*Transform, ItemCount, ItemType ���*/);

		
};
