// Fill out your copyright notice in the Description page of Project Settings.


#include "DateAssets/StartUpData/DataAsset_EnemyStartUpDataBase.h"
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<AbilitySystem/Abilities/WarriorEnemyGameplayAbility.h>
#include<GameplayAbilitySpec.h>
void UDataAsset_EnemyStartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel)
{
	Super::GiveToAbilitySystemComponent(InASC, ApplyLevel);
	if (!EnemyCombatAbilitySets.IsEmpty())
	{
		for (const auto& in : EnemyCombatAbilitySets)
		{
			if (in)
			{
				FGameplayAbilitySpec spec(in, ApplyLevel);
				spec.SourceObject = InASC->GetAvatarActor();
				InASC->GiveAbility(spec);
			}
		}
	}
}
