// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include "WarriorAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
class IPawnUIInterface;
UCLASS()
class PRACTICEDEMO_API UWarriorAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
	UWarriorAttributeSet();
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData CurrentHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentHealth);
	UPROPERTY(BlueprintReadOnly, Category = "Health")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxHealth);
	//耐力值：影响闪避以及冲刺
	UPROPERTY(BlueprintReadOnly, Category = "Staying")
	FGameplayAttributeData CurrentSTA;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentSTA);
	UPROPERTY(BlueprintReadOnly, Category = "Staying")
	FGameplayAttributeData MaxSTA;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxSTA);
	//每次格挡要消耗的基础的STA值
	UPROPERTY(BlueprintReadOnly, Category = "Staying")
	FGameplayAttributeData STACost_Block;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, STACost_Block);
	//每次格挡要消耗STA的系数
	UPROPERTY(BlueprintReadOnly, Category = "Staying")
	FGameplayAttributeData STACost_Coefficient;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, STACost_Coefficient);
	//怒气值：
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData CurrentRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, CurrentRage);
	UPROPERTY(BlueprintReadOnly, Category = "Rage")
	FGameplayAttributeData MaxRage;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, MaxRage);
	//影响造成的伤害
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, AttackPower);
	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FGameplayAttributeData DefensePower;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DefensePower);
	//伤害中专计算时修改DamageTaken，最后应用在HP上
	UPROPERTY(BlueprintReadOnly, Category = "Damage") 
	FGameplayAttributeData DamageTaken;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, DamageTaken);

	//重量，影响人物受击后被击退距离，以及能否被击飞
	UPROPERTY(BlueprintReadOnly, Category = "Weight")
	FGameplayAttributeData Weight;
	ATTRIBUTE_ACCESSORS(UWarriorAttributeSet, Weight);
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)override; 
private:
	TWeakInterfacePtr<IPawnUIInterface>CachedPawnUIInterface;

};
