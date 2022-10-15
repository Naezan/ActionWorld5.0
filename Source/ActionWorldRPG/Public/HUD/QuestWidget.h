// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestWidget.generated.h"

class APlayerBase;

UCLASS()
class ACTIONWORLDRPG_API UQuestWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void BindQuestDelegateFunction(APlayerBase* PlayerCharacter);

protected:
    //���� ����Ʈ�� �̸� �� ������ ª�� ǥ�����ݴϴ�.
    UFUNCTION(BlueprintCallable)
        void ShowActiveQuest();

    //���鿡 ū UI�� ??����Ʈ�Ϸᳪ ??����Ʈ + ����Ʈ������ ����մϴ�.
    UFUNCTION(BlueprintCallable)
        void ShowCurrentQuestState(bool bCompleted, FString QuestName);

    //HUD�� �����ʿ� ���� ����Ʈ ǥ�ø� �����մϴ�?
    UFUNCTION(BlueprintCallable)
        void EditActiveQuest(FString QuestName, FString QuestDesc);

    //HUD�� �����ʿ� ���� ����Ʈ�� �����ϳ����� �����մϴ�.
    UFUNCTION(BlueprintCallable)
        void EditActiveQuestStep();

    //HUD�� �����ʿ� ���� ����Ʈ ǥ�ø� �����մϴ�.
    UFUNCTION(BlueprintCallable)
        void HideActiveQuest();

    UFUNCTION(BlueprintCallable)
        void SpeakQuestTTS(TArray<FString> TTSText);

    //BP���� ����ϴ� �ؽ�Ʈ�� ����Ʈ ���� ���� �Լ��Դϴ�.
    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void ShowActiveQuestUpdate();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void ShowCurrentQuestStateUpdate(bool bCompleted, const FString& QuestName);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void EditActiveQuestUpdate(const FString& QuestName, const FString& QuestDesc);

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void EditActiveQuestStepUpdate();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void HideActiveQuestUpdate();

    UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
        void SpeakQuestTTSUpdate(const TArray<FString>& TTSText);

protected:
	UPROPERTY(BlueprintReadOnly)
		APlayerBase* Player;
};
