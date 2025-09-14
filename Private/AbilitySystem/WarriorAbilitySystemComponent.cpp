// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAbilitySystemComponent.h"
#include<DebugHelper/Debug.h>

#include<GameplayAbilitySpec.h>
#include<AbilitySystem/Abilities/WarriorGameplayAbility.h>
#include<AbilitySystem/Abilities/WarriorHeroGameplayAbility.h>
#include<WarriorGameplayTags.h>
void UWarriorAbilitySystemComponent::OnAbilityInputPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())return;
	for (const auto& in : GetActivatableAbilities())
	{ 
		if (!in.DynamicAbilityTags.HasTagExact(InputTag))continue;

		if (InputTag.MatchesTag(WarriorGameplayTags::InputTag_Toggleable)&& in.IsActive())
		{
			CancelAbilityHandle(in.Handle);
		}
		else
		{
			SetCurrentActivatedAbility(in.Handle);
			TryActivateAbility(in.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::OnAbilityInputReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid() || !InputTag.MatchesTag(WarriorGameplayTags::InputTag_MustBeHeld))
	{
		return;
	}
	for (const auto& in : GetActivatableAbilities())
	{
		if (in.DynamicAbilityTags.HasTagExact(InputTag) && in.IsActive())
		{
			CancelAbilityHandle(in.Handle);
		}
	}
}

void UWarriorAbilitySystemComponent::GrantHeroWeaponAbilities(const TArray<FWarriorHeroAbilitySet>& InWeaponAbilities, const TArray<FWarriorHeroSpecialAbilitySet>& InSpecialWeaponAbilities, int32 ApplyLevel, TArray<FGameplayAbilitySpecHandle>& OutGrantAbilitySpecHandles)
{

	if (!InWeaponAbilities.IsEmpty())
	{
		for (const auto& in : InWeaponAbilities)
		{
			if (!in.IsValid())return;
			FGameplayAbilitySpec spec(in.AbilityToGrant, ApplyLevel);
			spec.SourceObject = GetAvatarActor();
			spec.DynamicAbilityTags.AddTag(in.InputTag);
			OutGrantAbilitySpecHandles.AddUnique(GiveAbility(spec));
		}
	}
	if (!InSpecialWeaponAbilities.IsEmpty())
	{
		for (const auto& in : InSpecialWeaponAbilities)
		{
			if (!in.IsValid())return;
			FGameplayAbilitySpec spec(in.AbilityToGrant, ApplyLevel);
			spec.SourceObject = GetAvatarActor();
			spec.DynamicAbilityTags.AddTag(in.InputTag);
			OutGrantAbilitySpecHandles.AddUnique(GiveAbility(spec));
		}
	}
}


void UWarriorAbilitySystemComponent::RemoveGrantedHeroWeaponAbilities(UPARAM(ref)TArray<FGameplayAbilitySpecHandle>& InSpecHandlesToRemove)
{
	if (InSpecHandlesToRemove.IsEmpty())return;
	for (const auto& in : InSpecHandlesToRemove)
	{
		if (in.IsValid())
		{
			ClearAbility(in);
		}
	}
	InSpecHandlesToRemove.Empty();
}

bool UWarriorAbilitySystemComponent::TryActivateAbilityByTag(FGameplayTag AbilityTagToActivate)
{
	check(AbilityTagToActivate.IsValid());
	TArray<FGameplayAbilitySpec*>FoundActivatableAbilitieSpecs;
	GetActivatableGameplayAbilitySpecsByAllMatchingTags(AbilityTagToActivate.GetSingleTagContainer(), FoundActivatableAbilitieSpecs);
	if (!FoundActivatableAbilitieSpecs.IsEmpty())
	{
		const int32 RandomAbilityIndex = FMath::RandRange(0, FoundActivatableAbilitieSpecs.Num() - 1);
		FGameplayAbilitySpec*SpecToActivate = FoundActivatableAbilitieSpecs[RandomAbilityIndex];
		check(SpecToActivate);
		if (!SpecToActivate->IsActive())
		{
			return TryActivateAbility(SpecToActivate->Handle);
		}
	}
	return false;
}

void UWarriorAbilitySystemComponent::EndCurrentActivatedAbility()
{
	if (CurrentActivatingSpecHandle.IsValid())
	{
		CancelAbilityHandle(CurrentActivatingSpecHandle);
	}
}

void UWarriorAbilitySystemComponent::SetCurrentActivatedAbility(const FGameplayAbilitySpecHandle& InSpecHandle)
{
	CurrentActivatingSpecHandle = InSpecHandle;
}

FGameplayAbilitySpecHandle UWarriorAbilitySystemComponent::GetCurrentActivatedAbility() const
{
	return CurrentActivatingSpecHandle;
}




