// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "GEExeCalc_STACost.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UGEExeCalc_STACost : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UGEExeCalc_STACost();
	void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
