// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"

#include "Engine/DataAsset.h"

#include "ActionWorldAbilitySet.generated.h"

class UActionAbilitySystemComponent;
class UActionGameplayAbility;
class UGameplayEffect;

//�Ҵ�� �����Ƽ�� �����Ƽ �¿��� ����ϴ� ������
USTRUCT(BlueprintType)
struct FActionAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UActionGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
		int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
		FGameplayTag InputTag;
};

//�Ҵ�� ����Ʈ�� �����Ƽ�¿��� ����ϴ� ������
USTRUCT(BlueprintType)
struct FActionAbilitySet_GameplayEffect
{
	GENERATED_BODY()

public:

	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UGameplayEffect> GameplayEffect = nullptr;

	// Level of gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
		float EffectLevel = 1.0f;
};

//�Ҵ�� ��Ʈ����Ʈ������ �����Ƽ���� ����ϴ� ������
USTRUCT(BlueprintType)
struct FActionAbilitySet_AttributeSet
{
	GENERATED_BODY()

public:
	// Gameplay effect to grant.
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<UAttributeSet> AttributeSet;

};

//���� �ɷ� ���� �Ҵ�Ǿ����� ������ �� �ִ� �����ͷ� ���ȴ�.
USTRUCT(BlueprintType)
struct FActionAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);
	void AddGameplayEffectHandle(const FActiveGameplayEffectHandle& Handle);
	void AddAttributeSet(UAttributeSet* Set);

	void TakeFromAbilitySystem(UActionAbilitySystemComponent* ActionASC);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
		TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;

	// Handles to the granted gameplay effects.
	UPROPERTY()
		TArray<FActiveGameplayEffectHandle> GameplayEffectHandles;

	// Pointers to the granted attribute sets
	UPROPERTY()
		TArray<UAttributeSet*> GrantedAttributeSets;
};

/**
 * 
 */
UCLASS(BlueprintType, Const)
class ACTIONWORLDRPG_API UActionWorldAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	UActionWorldAbilitySet();

	// Grants the ability set to the specified ability system component.
	// The returned handles can be used later to take away anything that was granted.
	void GiveToAbilitySystem(UActionAbilitySystemComponent* LyraASC, FActionAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;

protected:

	// Gameplay abilities to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
		TArray<FActionAbilitySet_GameplayAbility> GrantedGameplayAbilities;

	// Gameplay effects to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects", meta = (TitleProperty = GameplayEffect))
		TArray<FActionAbilitySet_GameplayEffect> GrantedGameplayEffects;

	// Attribute sets to grant when this ability set is granted.
	UPROPERTY(EditDefaultsOnly, Category = "Attribute Sets", meta = (TitleProperty = AttributeSet))
		TArray<FActionAbilitySet_AttributeSet> GrantedAttributes;
};
