// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include<GameplayTagContainer.h>
#include "WarriorBaseAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UWarriorBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
protected:
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (BlueprintThreadSafe="true"))
	bool DoesOwnerHaveTag(FGameplayTag TagToCheck)const;
};
