// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/Inventory/ItemCollectorComponent.h"
#include "Items/CollectableItem.h"

// Sets default values for this component's properties
UItemCollectorComponent::UItemCollectorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UItemCollectorComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UItemCollectorComponent::FinishCollectingItem(AActor* CollectableActor)
{
	if (Cast<IMagnetableInterface>(CollectableActor))
	{
		Cast<ACollectableItem>(CollectableActor)->FinishCollect();
	}

}
