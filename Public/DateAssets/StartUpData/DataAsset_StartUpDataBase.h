// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "DataAsset_StartUpDataBase.generated.h"


class UWarriorGameplayAbility;
class UWarriorAbilitySystemComponent;
class UGameplayEffect;
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UDataAsset_StartUpDataBase : public UDataAsset
{
	GENERATED_BODY()
public: 
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent*InASC,const int32& ApplyLevel=1);


protected:
	//给予即触发
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData|Base")
	TArray<TSubclassOf<UWarriorGameplayAbility>>ActivateOnGivenAbilities;
	//通过Event触发的能力
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData|Base")
	TArray<TSubclassOf<UWarriorGameplayAbility>>ReactiveAbilities;
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData|Base")
	TArray<TSubclassOf<UGameplayEffect>>StartUpGameplayEffects;

	void GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>&InAbilitiesToGive, UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel = 1);
};
