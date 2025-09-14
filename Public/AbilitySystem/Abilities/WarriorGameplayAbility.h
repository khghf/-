// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include<GameplayEffectTypes.h>
#include "WarriorGameplayAbility.generated.h"
class UPawnCombatComponent;
class UWarriorAbilitySystemComponent;
UENUM(BlueprintType)
enum class EWarriorAbilityActivationPolicy :uint8
{
	OnTriggered ,
	OnGiven,
};
UCLASS()
class PRACTICEDEMO_API UWarriorGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, Category = "Warrior|Ability")
	EWarriorAbilityActivationPolicy AbilityActivationPolicy = EWarriorAbilityActivationPolicy::OnTriggered;

protected:
	//~ Begin UGameplayAbility Interface.
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
	//~ End UGameplayAbility Interface
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UPawnCombatComponent* GetPawnCombatComponentFromActorInfo()const; 
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponentFromActorInfo()const;

	FActiveGameplayEffectHandle NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (DisplayName = "Apply Effect Spec Handle To Target", ExpandEnumAsExecs = "OutSuccessType"))
	FActiveGameplayEffectHandle BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType&OutSuccessType);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (DisplayName = "ApplyGameplayEffectSpecHandleToHitResults"))
	void ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle,const TArray<FHitResult>&InHitResults);
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	FGameplayEffectSpecHandle MakeHasDurationEffectSpecHandle(TSubclassOf<UGameplayEffect>EffectClass, float DurationTime);
	//提供一个GE类即可返回一个修改属性的SpecHandle,用于一次性修改可配合MakeGameplayModifierInfo函数来简单配置修改器
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayEffectSpecHandle MakeInstantCostEffectSpecHandle(TSubclassOf<UGameplayEffect>EffectClass,TArray<FGameplayModifierInfo> Modifiers);
	
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FGameplayModifierInfo MakeGameplayModifierInfo(
		FGameplayAttribute Attribute,
		TEnumAsByte<EGameplayModOp::Type> ModifierOp,
		float ModifierMagnitude,
		FGameplayTagRequirements SourceTags,
		FGameplayTagRequirements TargetTags);
};
