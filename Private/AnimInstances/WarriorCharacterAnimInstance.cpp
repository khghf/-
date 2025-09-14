// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/WarriorCharacterAnimInstance.h"


#include<Characters/BaseCharacter.h>
#include<GameFramework/CharacterMovementComponent.h>
#include<KismetAnimationLibrary.h>
#include<Kismet/KismetMathLibrary.h>
void UWarriorCharacterAnimInstance::NativeInitializeAnimation()
{
	OwningCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (OwningCharacter)
	{
		OwningMovementComponent=OwningCharacter->GetCharacterMovement();
	}
}

void UWarriorCharacterAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	if (!OwningCharacter || !OwningMovementComponent)return;
	GroundSpeed = OwningMovementComponent->Velocity.Size2D();
	bHasAcceleration = OwningMovementComponent->GetCurrentAcceleration().SizeSquared2D() > 0.f;
	LocomotionDirection = UKismetAnimationLibrary::CalculateDirection(OwningCharacter->GetVelocity(), OwningCharacter->GetActorRotation());
	
}

ECardinalDirection UWarriorCharacterAnimInstance::SelectCardinalDirectionalFromAngle(const float& InAngle, const float& InDeadZone, const ECardinalDirection& InCurrentDirection, const bool& InbUseCurrentDirection)
{
	float ABSAngle = UKismetMathLibrary::Abs(InAngle);
	float ForwardDeadZone = InDeadZone;
	float BackwardDeadZone = InDeadZone;
	if (InbUseCurrentDirection)
	{
		//增大前/后检测范围
		switch (InCurrentDirection)
		{
		case ECardinalDirection::Forward:
			ForwardDeadZone *= 2.f;
			break;
		case ECardinalDirection::Backward:
			BackwardDeadZone *= 2.f;
			break;
		default:
			break;
		}
	}
	if (ABSAngle <= 45.f + ForwardDeadZone)return ECardinalDirection::Forward;
	if (ABSAngle >= 135.f - BackwardDeadZone)return ECardinalDirection::Backward;
	if (InAngle >0.f)return ECardinalDirection::Right;
	return ECardinalDirection::Left;
}
ECardinalDirection UWarriorCharacterAnimInstance::GetOppositeCaridinalDirection(const ECardinalDirection& InDirection)
{
	ECardinalDirection ret;
	switch (InDirection)
	{
	case ECardinalDirection::Forward:
		ret = ECardinalDirection::Backward;
		break;
	case ECardinalDirection::Backward:
		ret = ECardinalDirection::Forward;
		break;
	case ECardinalDirection::Left:
		ret = ECardinalDirection::Right;
		break;
	case ECardinalDirection::Right:
		ret = ECardinalDirection::Left;
		break;
	default:
		break;
	}
	return ret;
}