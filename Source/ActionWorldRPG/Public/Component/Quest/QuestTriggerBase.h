// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Component/Quest/QuestData.h"
#include "QuestTriggerBase.generated.h"

UCLASS()
class ACTIONWORLDRPG_API AQuestTriggerBase : public AActor
{
	GENERATED_BODY()

public:
	AQuestTriggerBase();

	virtual void BeginPlay() override;

	//일반적으로 트리거와 부딪히거나 상호작용했을때 호출됩니다.
	//Kill의 경우 BeginOverlap때 KillQuest가 추가됩니다. 위 함수는 호출되지 않습니다. 또는 Kill의경우 순차적으로 추가되는 로직과 동일하게 동작합니다.
	UFUNCTION(BlueprintCallable)
		void FinishQuest();

private:
	void SetQuestComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		EQuestType QuestType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FString QuestName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UQuestSystemComponent* QuestComponent;

	//만약 다음퀘스트가 순차적으로 이어질 경우 이 멤버변수를 사용합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* NextQuest;
};
