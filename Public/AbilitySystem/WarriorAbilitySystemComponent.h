// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include<GameplayTagContainer.h>
#include<WarriorTypes/WarriorStructTypes.h>
#include "WarriorAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UWarriorAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	void OnAbilityInputPressed(const FGameplayTag&InputTag);
	void OnAbilityInputReleased(const FGameplayTag& InputTag);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability", meta = (ApplyLevel="1"))
	void GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InWeaponAbilities, const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities,int32 ApplyLevel,TArray<FGameplayAbilitySpecHandle>&OutGrantAbilitySpecHandles);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	bool TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Ability")
	void EndCurrentActivatedAbility();
	void SetCurrentActivatedAbility(const FGameplayAbilitySpecHandle& CurrentActivatingSpecHandle);
	FGameplayAbilitySpecHandle GetCurrentActivatedAbility()const;
private:
	FGameplayAbilitySpecHandle CurrentActivatingSpecHandle;
};
