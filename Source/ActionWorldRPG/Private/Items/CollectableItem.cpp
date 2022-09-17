// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/CollectableItem.h"
#include "Components/SphereComponent.h"
#include "Component/Inventory/ItemCollectorComponent.h"

// Sets default values
ACollectableItem::ACollectableItem()
{
	PrimaryActorTick.bCanEverTick = false;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(ItemMesh);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBeginOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnEndOverlap);

	//Timeline
	FollowTargetTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FollowTargetTimeline"));
	FollowTargetFunction.BindUFunction(this, FName("OnMoveToTarget"));
	FollowTargetFinishEvent.BindUFunction(this, FName("OnMoveFinished"));
}

// Called when the game starts or when spawned
void ACollectableItem::BeginPlay()
{
	Super::BeginPlay();

	InitFollowTargetTimeline();
}

void ACollectableItem::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ItemCollectorComponent != nullptr)
	{
		return;
	}

	ItemCollectorComponent = OtherActor->FindComponentByClass<UItemCollectorComponent>();
	if (ItemCollectorComponent != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("BeginOverlap"));

		//아이템이 다 차있는 등 예외처리

		FollowTarget = OtherActor;
		FollowTargetTimeline->Play();
	}
}

void ACollectableItem::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ItemCollectorComponent == nullptr)
	{
		return;
	}

	if (OtherActor != ItemCollectorComponent->GetOwner())
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("EndOverlap"));
	FollowTargetTimeline->Stop();
	ItemCollectorComponent = nullptr;
	FollowTarget = nullptr;
}

void ACollectableItem::InitFollowTargetTimeline()
{
	if (FollowTargetCurveFloat)
	{
		FollowTargetTimeline->AddInterpFloat(FollowTargetCurveFloat, FollowTargetFunction);
		FollowTargetTimeline->SetTimelineFinishedFunc(FollowTargetFinishEvent);
		FollowTargetTimeline->SetLooping(false);
	}
}

void ACollectableItem::OnMoveToTarget(float DeltaTime)
{
	if (FollowTarget != nullptr)
	{
		FVector NewLocation = FMath::Lerp(GetActorLocation(), FollowTarget->GetActorLocation(), DeltaTime);
		SetActorLocation(NewLocation);
	}
}

void ACollectableItem::OnMoveFinished()
{
	if (ItemCollectorComponent != nullptr && FollowTarget != nullptr)
	{
		ItemCollectorComponent->FinishCollectingItem(this);

		ItemCollectorComponent = nullptr;
		FollowTarget = nullptr;

		Destroy();
	}
}
