// Fill out your copyright notice in the Description page of Project Settings.

#include "Characters/BaseCharacter.h"
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<AbilitySystem/WarriorAttributeSet.h>
#include<MotionWarpingComponent.h>

#include<DebugHelper/Debug.h>
// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;
	GetMesh()->bReceivesDecals = false;
	WarriorAbilitySystemComponent = CreateDefaultSubobject<UWarriorAbilitySystemComponent>("WarriorAbilitySystemComponent");
	WarriorAttributeSet =CreateDefaultSubobject<UWarriorAttributeSet>("WarriorAttributeSet");
	MotionWarpingComponent= CreateDefaultSubobject<UMotionWarpingComponent>("MotionWarpingComponent");

}



void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (WarriorAbilitySystemComponent)
	{
		WarriorAbilitySystemComponent->InitAbilityActorInfo(this, this);
		ensureMsgf(!CharacterStartUpdata.IsNull(), TEXT("Forgot to assgin start uo data to %s"),*GetName());
	}
}
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return GetWarriorAbilitySystemComponent();
}
UAttributeSet* ABaseCharacter::GetAttributeSet() const
{
	return GetWarriorAttributeSet();
}

UPawnCombatComponent* ABaseCharacter::GetPawnCombatComponent() const
{
	return nullptr;
}

UPawnUIComponent* ABaseCharacter::GetPawnUIComponent() const
{
	return nullptr;
}

