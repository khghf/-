// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include<GameplayTagContainer.h>
#include "DataAsset_InputConfig.generated.h"

class UInputAction;
class UInputMappingContext;
USTRUCT(BlueprintType)
struct FWarriorInputActionConfig
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputAction;
	bool IsValid()const
	{
		return InputTag.IsValid() && InputAction;
	}
};


UCLASS()
class PRACTICEDEMO_API UDataAsset_InputConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UInputAction* FindNativeInputActionByTag(const FGameplayTag& InInputTag)const;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty="InputTag"))
	TArray<FWarriorInputActionConfig>NativeInputAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorInputActionConfig>AbilityInputAction;
	
};
