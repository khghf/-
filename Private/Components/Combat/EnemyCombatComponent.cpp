// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Combat/EnemyCombatComponent.h"


#include<AbilitySystemBlueprintLibrary.h>
#include<WarriorGameplayTags.h>
#include<WarriorFunctionLibrary.h>
#include<Characters/EnemyCharacter.h>
#include<Components/BoxComponent.h>
#include<DebugHelper/Debug.h>
void UEnemyCombatComponent::OnHitTargetActor(AActor* HitActor)
{
	if (OverlapedActors.Contains(HitActor))return;
	OverlapedActors.AddUnique(HitActor);
	const bool bAttackWasDodged= UWarriorFunctionLibrary::NativeDoesActorHasTag(HitActor, WarriorGameplayTags::Player_Status_Dodging_Invinciable);
	bool bIsValidBlock = false;
	bool bIsPlayerBlocking = UWarriorFunctionLibrary::NativeDoesActorHasTag(HitActor,WarriorGameplayTags::Player_Status_Blocking);
	const bool bIsMyAttachUnblockable = UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwningPawn(), WarriorGameplayTags::Enemy_Status_Unblockable);
	
	FGameplayEventData Data;
	Data.Instigator = GetOwningPawn();
	Data.Target = HitActor;
	if (bAttackWasDodged)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_PerfectDodge,
			Data
		);
		return;
	}
	if (bIsPlayerBlocking && !bIsMyAttachUnblockable)
	{
		bIsValidBlock=UWarriorFunctionLibrary::IsValidBlock(GetOwningPawn(), HitActor);
	}
	if (bIsValidBlock==true)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			HitActor,
			WarriorGameplayTags::Player_Event_SuccessfulBlock,
			Data
		);
	}
	else 
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			GetOwningPawn(),
			WarriorGameplayTags::Shared_Event_MeleeHit,
			Data
		);
	}
}

void UEnemyCombatComponent::OnWeaponPulledFromTargetActor(AActor* InteractedActor)
{
}

void UEnemyCombatComponent::ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType)
{
	AEnemyCharacter* OwningEnemyCharacter = GetOwningPawn< AEnemyCharacter>();
	check(OwningEnemyCharacter);
	UBoxComponent* LeftHandCollisionBox = OwningEnemyCharacter->GetLeftHandCollisionBox();
	UBoxComponent* RightHandCollisionBox = OwningEnemyCharacter->GetRightHandCollisionBox();
	check(LeftHandCollisionBox && RightHandCollisionBox);
	switch (ToggleDamageType)
	{
	case EToggleDamageType::CurrentWeapon:
		break;
	case EToggleDamageType::LeftHand:
		LeftHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;
	case EToggleDamageType::RightHand:
		RightHandCollisionBox->SetCollisionEnabled(bShouldEnable ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
		break;
	default:
		break;
	}
	if (!bShouldEnable)
	{
		OverlapedActors.Empty();
	}
}
