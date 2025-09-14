// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/PickUps/WarriorStoneBase.h"


#include<Characters/HeroCharacter.h>
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<WarriorGameplayTags.h>
void AWarriorStoneBase::Consume(UWarriorAbilitySystemComponent* ASC, int32 ApplyLevel)
{
	check(StoneGameplayEffectClass);
	UGameplayEffect* EffectCDO = StoneGameplayEffectClass->GetDefaultObject<UGameplayEffect>();
	ASC->ApplyGameplayEffectToSelf(
		EffectCDO,
		ApplyLevel,
		ASC->MakeEffectContext()
	);
	BP_OnStoneConsumed();
}
void AWarriorStoneBase::OnPickUpCollisionSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AHeroCharacter* OverlapedHeroCharacter = Cast<AHeroCharacter>(OtherActor))
	{
		OverlapedHeroCharacter->GetWarriorAbilitySystemComponent()->TryActivateAbilityByTag(WarriorGameplayTags::Player_Ability_PickUp_Stones);
	}
	
}
