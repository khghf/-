// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include "WarriorEnemyGameplayAbility.generated.h"
class AEnemyCharacter;
class UEnemyCombatComponent;
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UWarriorEnemyGameplayAbility : public UWarriorGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	AEnemyCharacter* GetEnemyCharacterFromActorInfo();
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	UEnemyCombatComponent* GetEnemyCombatComponentFromActorInfo();
	UFUNCTION(BlueprintPure, Category = "Warrior|Ability")
	FGameplayEffectSpecHandle MakeEnemyDamageEffectSpecHandle(TSubclassOf<UGameplayEffect>EffectClass,const FScalableFloat&InDamageScalableFloat);
private:
	TWeakObjectPtr<AEnemyCharacter>CachedEnemyCharacter;
	//TWeakObjectPtr<ACombatPlayerController>CachedCombatPlayerController;
};
