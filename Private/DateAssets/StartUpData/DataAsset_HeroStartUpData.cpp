// Fill out your copyright notice in the Description page of Project Settings.


#include "DateAssets/StartUpData/DataAsset_HeroStartUpData.h"


#include<AbilitySystem/Abilities/WarriorGameplayAbility.h>
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<AbilitySystem/Abilities/WarriorHeroGameplayAbility.h>
#include<WarriorTypes/WarriorStructTypes.h>
void UDataAsset_HeroStartUpData::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASC, ApplyLevel);
	for (const auto& in : HeroStartUpAbilitySets)
	{
		if (in.IsValid())
		{
			FGameplayAbilitySpec spec(in.AbilityToGrant, ApplyLevel);
			spec.SourceObject = InASC->GetAvatarActor();
			//后续触发时需要根据DynamicAbilityTags拥有的标签来判断
			spec.DynamicAbilityTags.AddTag(in.InputTag);
			InASC->GiveAbility(spec);
		}
	}
}
