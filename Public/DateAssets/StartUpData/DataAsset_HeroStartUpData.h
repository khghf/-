// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DateAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include<GameplayTagContainer.h>
#include "DataAsset_HeroStartUpData.generated.h"

struct FWarriorHeroAbilitySet;
UCLASS() 
class PRACTICEDEMO_API UDataAsset_HeroStartUpData : public UDataAsset_StartUpDataBase
{
	GENERATED_BODY()
public:
	virtual void GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel = 1)override;
private:
	UPROPERTY(EditDefaultsOnly, Category = "StartUpData|Hero", meta = (TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet>HeroStartUpAbilitySets;
};
