// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/PawnCombatComponent.h"
#include<Items/Weapons/WarriorWeaponBase.h>
#include<Components/BoxComponent.h>
void UPawnCombatComponent::RegisterSpwanedWeapon(FGameplayTag InWeaponTag, AWarriorWeaponBase* InWeapon, bool bRegisterAsEquippedWeapon)
{
	checkf(!CharacterCarriedWeaponMap.Contains(InWeaponTag), TEXT("A named weapon '%s' has already added as carried weapon"), *InWeaponTag.ToString());
	check(InWeapon);
	CharacterCarriedWeaponMap.Emplace(InWeaponTag, InWeapon);
	if (bRegisterAsEquippedWeapon)
	{
		CurrentEquippedWeaponTag = InWeaponTag;
	}
	InWeapon->OnWeaponHitTarget.BindUObject(this, &ThisClass::OnHitTargetActor);
	InWeapon->OnWeaponPulledFromTarget.BindUObject(this, &ThisClass::OnWeaponPulledFromTargetActor);
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTag) const
{
	if (CharacterCarriedWeaponMap.Contains(InWeaponTag))
	{
		if (AWarriorWeaponBase* const* FindWeapon = CharacterCarriedWeaponMap.Find(InWeaponTag))
		{
			return *FindWeapon;
		}
	}
	return nullptr;
}

AWarriorWeaponBase* UPawnCombatComponent::GetCharacterCurrentEquippedWeapon() const
{
	if (CurrentEquippedWeaponTag.IsValid())
	{
		return GetCharacterCarriedWeaponByTag(CurrentEquippedWeaponTag);
	}
	return nullptr;
}

void UPawnCombatComponent::ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	if (ToggleDamageType == EToggleDamageType::CurrentWeapon)
	{
		ToggleCurrentEquippedWeaponCollision(bShouldEnable);
	}
	else 
	{
		ToggleBodyCollisionBoxCollision(bShouldEnable, ToggleDamageType);
	}
}

void UPawnCombatComponent::OnHitTargetActor(AActor* HitActor)
{

}

void UPawnCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}

void UPawnCombatComponent::ToggleCurrentEquippedWeaponCollision(bool bShouldEnable)
{
	AWarriorWeaponBase* WeaponToToggle = GetCharacterCurrentEquippedWeapon();
	check(WeaponToToggle);
	if (bShouldEnable)
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		WeaponToToggle->GetWeaponCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		OverlapedActors.Empty();
	}
}

void UPawnCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
}

