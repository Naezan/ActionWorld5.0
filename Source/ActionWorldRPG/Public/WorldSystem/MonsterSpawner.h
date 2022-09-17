// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/WorldInteractionInterface.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class ACTIONWORLDRPG_API AMonsterSpawner : public AActor, public IWorldInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMonsterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void WorldInteractionProcess_Implementation();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
	class UBoxComponent* BoxComponent;

	//�����߿� �������� �ϳ� �����ؼ� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
		TArray<TSubclassOf<class AMonsterBase>> Monsters;

	//��ŭ �����Ұ�����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawner")
		int32 MaxMonsterCount;
};
