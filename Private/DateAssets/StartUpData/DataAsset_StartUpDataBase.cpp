// Fill out your copyright notice in the Description page of Project Settings.


#include "DateAssets/StartUpData/DataAsset_StartUpDataBase.h"


#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<AbilitySystem/Abilities/WarriorGameplayAbility.h>
#include<GameplayAbilitySpec.h>
#include<GameplayEffect.h>

void UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent(UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel)
{
	checkf(InASC,TEXT("UDataAsset_StartUpDataBase::GiveToAbilitySystemComponent::InASC invalid"));
	GrantAbilities(ActivateOnGivenAbilities, InASC, ApplyLevel);
	GrantAbilities(ReactiveAbilities, InASC, ApplyLevel);
	if (!StartUpGameplayEffects.IsEmpty())
	{
		for (const auto& in : StartUpGameplayEffects)
		{
			if (in)
			{
				UGameplayEffect* GECDO = in->GetDefaultObject<UGameplayEffect>();
				InASC->ApplyGameplayEffectToSelf(
					GECDO,
					ApplyLevel,
					InASC->MakeEffectContext()
					);
			}
		}
	}
}

void UDataAsset_StartUpDataBase::GrantAbilities(const TArray<TSubclassOf<UWarriorGameplayAbility>>& InAbilitiesToGive, UWarriorAbilitySystemComponent* InASC, const int32& ApplyLevel)
{
	if (InAbilitiesToGive.IsEmpty())return;
	for (const auto& in : InAbilitiesToGive)
	{
		if (IsValid(in))
		{
			FGameplayAbilitySpec ASpec(in, ApplyLevel);
			ASpec.SourceObject = InASC->GetAvatarActor();
			InASC->GiveAbility(ASpec);
		}
	}
}
