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
    //현재 퀘스트의 이름 및 내용을 짧게 표시해줍니다.
    UFUNCTION(BlueprintCallable)
        void ShowActiveQuest();

    //전면에 큰 UI로 ??퀘스트완료나 ??퀘스트 + 퀘스트내용을 출력합니다.
    UFUNCTION(BlueprintCallable)
        void ShowCurrentQuestState(bool bCompleted, FString QuestName);

    //HUD의 오른쪽에 현재 퀘스트 표시를 수정합니다?
    UFUNCTION(BlueprintCallable)
        void EditActiveQuest(FString QuestName, FString QuestDesc);

    //HUD의 오른쪽에 현재 퀘스트의 디테일내용을 수정합니다.
    UFUNCTION(BlueprintCallable)
        void EditActiveQuestStep();

    //HUD의 오른쪽에 현재 퀘스트 표시를 삭제합니다.
    UFUNCTION(BlueprintCallable)
        void HideActiveQuest();

    UFUNCTION(BlueprintCallable)
        void SpeakQuestTTS(TArray<FString> TTSText);

    //BP에서 사용하는 텍스트나 퀘스트 내용 변경 함수입니다.
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
