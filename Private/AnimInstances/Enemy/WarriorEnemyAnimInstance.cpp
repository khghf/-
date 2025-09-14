// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Enemy/WarriorEnemyAnimInstance.h"

#include<Characters/EnemyCharacter.h>
#include<GameFramework/CharacterMovementComponent.h>
#include<Kismet/KismetMathLibrary.h>
#include<WarriorFunctionLibrary.h>
#include<WarriorGameplayTags.h>
UWarriorEnemyAnimInstance::UWarriorEnemyAnimInstance()
{
	bUseMultiThreadedAnimationUpdate = true;
}

void UWarriorEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwningCharacter)
	{
		OwnningEnemyCharacter = Cast<AEnemyCharacter>(OwningCharacter);
	}
}

void UWarriorEnemyAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!OwnningEnemyCharacter || !OwningMovementComponent)return;
	DeltaTime = DeltaSeconds;
	/*if (bHasAcceleration)
	{
		IdleElpasedTime = 0.f;
		bShouldEnterRelaxState = false;
	}
	else
	{
		IdleElpasedTime += DeltaSeconds;
		bShouldEnterRelaxState = IdleElpasedTime >= EnterRelaxStateThreshold;
	}*/
	UpdateLocationData();
	UpdateRotationData();
	UpdateVelocityData();
	//UpdateCharacterMovementState();
	//UpdateGroundDistance();
	bIsFirstUpdate = false;
}
void UWarriorEnemyAnimInstance::UpdateLocationData()
{

	if (bIsFirstUpdate)return;
	DisplacementSinceLastUpdate = (OwnningEnemyCharacter->GetActorLocation() - WorldLocation).Size2D();
	WorldLocation = OwnningEnemyCharacter->GetActorLocation();
	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, DeltaTime);
}

void UWarriorEnemyAnimInstance::UpdateRotationData()
{
	DeltaRotation = (OwningCharacter->GetActorRotation() - WorldRotation).Yaw;
	WorldRotation = OwningCharacter->GetActorRotation();
	if (bIsFirstUpdate)
	{
		DeltaRotation = 0.f;
	}

}

void UWarriorEnemyAnimInstance::UpdateVelocityData()
{
	bWasMovingLastUpdate = !LocalVelocity2D.IsNearlyZero();
	bHasAcceleration = ((OwningMovementComponent->Velocity - WorldVelocity) / DeltaTime).SizeSquared2D() > 0.f;
	WorldVelocity = OwningMovementComponent->Velocity;
	WorldVelocity2D = FVector(WorldVelocity.X, WorldVelocity.Y, 0.f);
	LocalVelocity2D = UKismetMathLibrary::Quat_UnrotateVector(WorldRotation.Quaternion(), WorldVelocity2D);
	bHasVelocity = UKismetMathLibrary::NearlyEqual_FloatFloat(LocalVelocity2D.Size2D(), 0.f);
	LocalVelocityDirectionAngle = UKismetMathLibrary::NormalizedDeltaRotator(WorldVelocity.Rotation(), WorldRotation).Yaw;
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	LocalVelocityDirection = SelectCardinalDirectionalFromAngle(LocalVelocityDirectionAngleWithOffset, 10.f, LocalVelocityDirection, bWasMovingLastUpdate);
	LocalVelocityDirectionNoOffset = SelectCardinalDirectionalFromAngle(LocalVelocityDirectionAngle, 10.f, LocalVelocityDirectionNoOffset, bWasMovingLastUpdate);
}



void UWarriorEnemyAnimInstance::UpdateCharacterMovementState()
{
	bIsOnGround = OwningMovementComponent->IsMovingOnGround();
	bIsFalling = bIsJumping = false;
	if (OwningMovementComponent->MovementMode == EMovementMode::MOVE_Falling)
	{
		if (OwningMovementComponent->Velocity.Z > 0)
		{
			bIsJumping = true;
		}
		else
		{
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(OwningCharacter, WarriorGameplayTags::Player_Status_Jumping);
			bIsFalling = true;
		}
	}
}

void UWarriorEnemyAnimInstance::UpdateGroundDistance()
{
	if (bIsFalling)
	{
		FFindFloorResult Res;
		OwningMovementComponent->ComputeFloorDist(OwningCharacter->GetActorLocation(), 0.f, 10000.f, Res, 1.f);
		GroundDistance = Res.FloorDist;
		return;
	}
	GroundDistance = 10000.f;
}