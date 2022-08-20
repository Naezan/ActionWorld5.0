#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemCollectorComponent.generated.h"

//플레이어가 자성을 가진 액터를 자동으로 수집하는 기능을 담당합니다.
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ACTIONWORLDRPG_API UItemCollectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemCollectorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//플레이어에게 추가해주는 기능을 합니다.
	void FinishCollectingItem(AActor* CollectableActor);
		
};
