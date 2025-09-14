// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DateAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include "DataAsset_EnemyStartUpDataBase.generated.h"

class UWarriorEnemyGameplayAbility;
UCLASS()
class PRACTICEDEMO_API UDataAsset_EnemyStartUpDataBase : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel = 1)override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData|Enemy", meta = (TitleProperty = "InputTag"))
	TArray<TSubclassOf<UWarriorEnemyGameplayAbility>>EnemyCombatAbilitySets;
};
