// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GEExecCalc/GEExecCalc_DamageTaken.h"



#include<AbilitySystem/WarriorAttributeSet.h>
#include "WarriorGameplayTags.h"

struct FWarriorDamageCapture
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(AttackPower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DefensePower)
	DECLARE_ATTRIBUTE_CAPTUREDEF(DamageTaken)
	FWarriorDamageCapture()
	{ 
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, AttackPower, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DefensePower, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UWarriorAttributeSet, DamageTaken, Target, false);
	}
};
static const FWarriorDamageCapture&GetWarriorDamageCapture()
{
	static FWarriorDamageCapture WarriorDamageCapture;
	return WarriorDamageCapture;
}
UGEExecCalc_DamageTaken::UGEExecCalc_DamageTaken()
{
	/*∏¥‘”–¥∑®
	FProperty* AttackPowerProperty=FindFieldChecked<FProperty>(
		UWarriorAttributeSet::StaticClass(),
		GET_MEMBER_NAME_CHECKED(UWarriorAttributeSet, AttackPower)
	);
	FGameplayEffectAttributeCaptureDefinition AttackPowerCaptureDefinition(
		AttackPowerProperty,
		EGameplayEffectAttributeCaptureSource::Source,
		false
	);
	RelevantAttributesToCapture.Add(AttackPowerCaptureDefinition);
	*/

	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().AttackPowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DefensePowerDef);
	RelevantAttributesToCapture.Add(GetWarriorDamageCapture().DamageTakenDef);
}

void UGEExecCalc_DamageTaken::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const FGameplayEffectSpec& EffectSpec= ExecutionParams.GetOwningSpec();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = EffectSpec.CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = EffectSpec.CapturedTargetTags.GetAggregatedTags();

	float SourceAttackPower=0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorDamageCapture().AttackPowerDef,
		EvaluateParameters,
		SourceAttackPower
	);
	float BaseDamage = 0.f;
	int32 UsedLightAttackComboCoutn = 0;
	int32 UsedHeavyAttackComboCoutn = 0;
	for (const auto& in : EffectSpec.SetByCallerTagMagnitudes)
	{
		if (in.Key.MatchesTagExact(WarriorGameplayTags::Shared_SetByCaller_BaseDamage))
		{
			BaseDamage = in.Value;
			//BaseDamage=*EffectSpec.SetByCallerTagMagnitudes.Find(WarriorGameplayTags::Shared_SetByCaller_BaseDamage);
		}
		if (in.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Light))
		{
			UsedLightAttackComboCoutn = in.Value;
		}
		if (in.Key.MatchesTagExact(WarriorGameplayTags::Player_SetByCaller_AttackType_Heavy))
		{
			UsedHeavyAttackComboCoutn = in.Value;
		}
	}

	float TargetDefensePower = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(
		GetWarriorDamageCapture().DefensePowerDef,
		EvaluateParameters,
		TargetDefensePower
	);

	if (UsedLightAttackComboCoutn != 0)
	{
		const float DamageIncreasePercentLight = (UsedLightAttackComboCoutn - 1) * 0.05 + 1.f;
		BaseDamage *= DamageIncreasePercentLight;
	}
	if (UsedHeavyAttackComboCoutn != 0)
	{
		const float DamageIncreasePercentHeavy = (UsedHeavyAttackComboCoutn - 1) * 0.15 + 1.f;
		BaseDamage *= DamageIncreasePercentHeavy;
	}
	const float FinalDamageDone = BaseDamage * SourceAttackPower / TargetDefensePower;
	if (FinalDamageDone > 0.f)
	{
		OutExecutionOutput.AddOutputModifier(
			FGameplayModifierEvaluatedData(
				GetWarriorDamageCapture().DamageTakenProperty,
				EGameplayModOp::Override,
				FinalDamageDone
			)
		);
	}
}
