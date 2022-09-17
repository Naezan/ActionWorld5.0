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

	//�Ϲ������� Ʈ���ſ� �ε����ų� ��ȣ�ۿ������� ȣ��˴ϴ�.
	//Kill�� ��� BeginOverlap�� KillQuest�� �߰��˴ϴ�. �� �Լ��� ȣ����� �ʽ��ϴ�. �Ǵ� Kill�ǰ�� ���������� �߰��Ǵ� ������ �����ϰ� �����մϴ�.
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

	//���� ��������Ʈ�� ���������� �̾��� ��� �� ��������� ����մϴ�.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UDataTable* NextQuest;
};
