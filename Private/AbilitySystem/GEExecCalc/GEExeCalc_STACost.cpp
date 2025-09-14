// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GEExecCalc/GEExeCalc_STACost.h"
#include<DebugHelper/Debug.h>

#include<AbilitySystem/WarriorAttributeSet.h>


struct FWarriorSATCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(STACost_Coefficient)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CurrentSTA)
	DECLARE_ATTRIBUTE_CAPTUREDEF(MaxSTA)
	FWarriorSATCapture()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, STACost_Coefficient, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, CurrentSTA, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, MaxSTA, Target, false);
	}
};
static const FWarriorSATCapture& GetWarriorSATCapture()
{
	static FWarriorSATCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}
UGEExeCalc_STACost::UGEExeCalc_STACost()
{
	RelevantAttributesToCapture.Add(GetWarriorSATCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorSATCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorSATCapture().STACost_CoefficientDef);
	RelevantAttributesToCapture.Add(GetWarriorSATCapture().CurrentSTADef);
	RelevantAttributesToCapture.Add(GetWarriorSATCapture().MaxSTADef);
}
void UGEExeCalc_STACost::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectSpec EffectSpec=ExecutionParams.GetOwningSpec();
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();
	float AttackerPower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorSATCapture().AttackPowerDef,
		EvaluateParameters,
		AttackerPower
	);
	float BlockerDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorSATCapture().DefensePowerDef,
		EvaluateParameters,
		BlockerDefensePower
	);
	float CurrentSTA = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorSATCapture().CurrentSTADef,
		EvaluateParameters,
		CurrentSTA
	);
	float MaxSTA = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorSATCapture().MaxSTADef,
		EvaluateParameters,
		MaxSTA
	);
	float FinalCoefficient = AttackerPower / BlockerDefensePower + (1 - CurrentSTA / MaxSTA);
	//const FString text = FString::Printf(TEXT("FinalCoefficient:%f"), FinalCoefficient);
	//Debug::Print(text);
	OutExecutionOutput.AddOutputModifier(
		FGameplayModifierEvaluatedData(
			GetWarriorSATCapture().STACost_CoefficientProperty,
			EGameplayModOp::Override,
			FinalCoefficient
		)
	);
}
