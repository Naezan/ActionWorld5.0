// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/InteractableActor.h"
#include "AbilitySystem/ActionAbilitySystemComponent.h"

//��������
#include "Components/GameFrameworkComponentManager.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UActionAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
}

UAbilitySystemComponent* AInteractableActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AInteractableActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
	
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AInteractableActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);

	Super::EndPlay(EndPlayReason);
}

void AInteractableActor::InteractWithInstigator_Implementation(AActor* Interactor)
{
	//���⼱ �������̵����� C++�Լ��� �������� �ʽ��ϴ�. ����·� �Ӵϴ�.
}

void AInteractableActor::InteractDelayWithInstigator_Implementation(AActor* Interactor)
{
	//���⼱ �������̵����� C++�Լ��� �������� �ʽ��ϴ�. ����·� �Ӵϴ�.
}

void AInteractableActor::SendToInstigator_Implementation(AActor* Interactor)
{
	//���⼱ �������̵����� C++�Լ��� �������� �ʽ��ϴ�. ����·� �Ӵϴ�.
}
