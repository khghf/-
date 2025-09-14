// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include<queue>
#include<GameplayTagContainer.h>
#include<WarriorTypes/WarriorEnumTypes.h>
#include "WarriorPreInputBufferComponent.generated.h"
DECLARE_DELEGATE_OneParam(FOnBufferClosed, FGameplayTag);
USTRUCT(BlueprintType)
struct FCacheUnit
{
	GENERATED_BODY()
	explicit FCacheUnit(const FGameplayTag& InInputTag, const int32& InPriority)
	{
		InputTag = InInputTag;
		Priority = InPriority;
	}
	FCacheUnit() = default;
	UPROPERTY(BlueprintReadOnly,Category = "InputTag")
	FGameplayTag InputTag;
	UPROPERTY()
	int32 Priority;
	bool operator()(const FCacheUnit& A, const FCacheUnit& B)
	{
		return A.Priority < B.Priority;
	}
};
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UWarriorPreInputBufferComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
private:
	std::priority_queue<FCacheUnit,std::vector<FCacheUnit>, FCacheUnit>StoredInput;
	bool bIsBufferOpen = false;
	UPROPERTY(EditDefaultsOnly, Category = "InputPriorityConfig")
	TMap<FGameplayTag, int32>InputPriorityConfig;
	FGameplayTag TagToConsume;
public:
	UFUNCTION(BlueprintCallable)
	void OpenBuffer();
	UFUNCTION(BlueprintCallable)
	void CloseBuffer();
	UFUNCTION(BlueprintCallable, meta = (ExpandEnumAsExecs = "OutSuccessType"))
	void ConsumeBuffer(EWarriorSuccessType&OutSuccessType);
	//只有在InputPriorityConfig配置的值才能被存储
	void StoreInput(const FGameplayTag& InInputTag);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool BufferIsEmpty()const;
	UFUNCTION(BlueprintCallable,BlueprintPure)
	bool TagToConsumeIsNone()const;
	FOnBufferClosed OnBufferConsumed;
private:
	void ClearStoredInput();
};
