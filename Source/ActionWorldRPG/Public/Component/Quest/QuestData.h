// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/DataAsset.h"
#include "QuestData.generated.h"

UENUM(BlueprintType)
enum class EQuestType : uint8
{
	QT_Move UMETA(DisplayName = "Move"),
	QT_Kill UMETA(DisplayName = "Kill"),
	QT_Interact UMETA(DisplayName = "Interact"),
	QT_None UMETA(DisplayName = "None"),
};

USTRUCT(BlueprintType)
struct FReward
{
	GENERATED_USTRUCT_BODY()

	//Reward Data -> InventoryItem, RewardCount
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	TMap<TSubclassOf<class UInventoryItem>, int32> RewardData;

	//보상으로 다음퀘스트가 존재한다면 다음퀘스트를 받는다. 이때 다음퀘스트가 있어도 보상을 받을수 있다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* NextQuest;

	//비어있는지 체크
	UPROPERTY(VisibleAnywhere, Category = "Reward")
		bool bIsEmpty = true;

	FReward();

	void Clear();
	bool IsEmpty();
};

USTRUCT(BlueprintType)
struct FQuest : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		FString QeustDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		TSubclassOf<AActor> TargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		int32 TargetCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		int32 MaxTargetCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		TSubclassOf<AActor> CompleteTargetClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		FReward Reward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		TArray<FString> QuestAddedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		TArray<FString> QuestCompletedText;

	//퀘스트르 현재 활성화되어있는지 사용안할수도 있다
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		bool bActive = false;

	//비어있는지 체크
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		bool bIsEmpty = true;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		bool bComplete = false;

	FQuest();

	void Clear();
	bool IsEmpty();
};

UCLASS(BlueprintType)
class ACTIONWORLDRPG_API UQuestData : public UDataAsset
{
	GENERATED_BODY()

public:
	//퀘스트들
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest")
		TArray<FQuest> QuestDatas;
};
