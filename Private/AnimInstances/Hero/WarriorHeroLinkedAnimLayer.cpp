// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroLinkedAnimLayer.h"
#include<AnimInstances/Hero/WarriorHeroAnimInstance.h>
#include<GameFramework/CharacterMovementComponent.h>
UWarriorHeroAnimInstance* UWarriorHeroLinkedAnimLayer::GetHeroAnimInstance() const
{
    return Cast<UWarriorHeroAnimInstance>(GetOwningComponent()->GetAnimInstance());
}

UCharacterMovementComponent* UWarriorHeroLinkedAnimLayer::GetCharacterMovementComponent() const
{
    if (APawn* OwnerPawn = TryGetPawnOwner())
    {
        return Cast<UCharacterMovementComponent>(OwnerPawn->GetMovementComponent());
    }
    return nullptr;
}

