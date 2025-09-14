// Fill out your copyright notice in the Description page of Project Settings.


#include "RootMotionModifer/RootMotionModifier_ATKRotation.h"


#include<GameFramework/CharacterMovementComponent.h>
#include<GameFramework/Character.h>
#include<WarriorGameplayTags.h>
#include<WarriorFunctionLibrary.h>
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include<BehaviorTree/BlackboardComponent.h>
#include<Kismet/KismetMathLibrary.h>
URootMotionModifier_ATKRotation::URootMotionModifier_ATKRotation(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bUseDegreeThreshold = false;
	bWarpTranslation = false;
	DegreeThreshold = 30.f;
	LastInputVector = FVector::ZeroVector;
}

void URootMotionModifier_ATKRotation::OnStateChanged(ERootMotionModifierState LastState)
{
	if (LastState != ERootMotionModifierState::Active && ERootMotionModifierState::Active == GetState())
	{
		LastInputVector = FVector::ZeroVector;
		if (ACharacter* CharacterOwner = GetCharacterOwner())
		{
			if (const UCharacterMovementComponent* CharacterMovement = CharacterOwner->GetCharacterMovement())
			{
				if (bForAI)
				{
					if (UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(CharacterOwner))
					{
						AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKeyName));
						checkf(TargetActor, TEXT("Forgot to assign a value to 'TargetKeyName' or TargetKey not a Actor"));
						FVector TargetLocation = TargetActor->GetActorLocation();
						FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(CharacterOwner->GetActorLocation(), TargetLocation);
						LastInputVector = FRotator(0.f, LookAt.Yaw, 0.f).Vector();
						
					}
				}
				else
				{
					LastInputVector = CharacterMovement->GetLastInputVector();
				}
				LastInputVector.Normalize();
			}

			if (bUseDegreeThreshold && !LastInputVector.IsNearlyZero())
			{
				//计算需要旋转的角度
				FVector ForwardVector = CharacterOwner->GetActorForwardVector();

				float ForwardCosAngle = FVector::DotProduct(ForwardVector, LastInputVector);
				float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

				float AngleToRotate = ForwardDeltaDegree - DegreeThreshold;

				if (AngleToRotate > 0.f)
				{
					FVector Axis = FVector::CrossProduct(ForwardVector, LastInputVector);
					Axis.Normalize();

					//right
					if (Axis.Z > 0)
					{
						float Alpha = DegreeThreshold / ForwardDeltaDegree;
						FQuat TotalRotation = FQuat::FindBetweenNormals(ForwardVector, LastInputVector);
						TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation, Alpha);
						LastInputVector = TotalRotation.RotateVector(ForwardVector);
					}
					//left
					else
					{
						float Alpha = DegreeThreshold / ForwardDeltaDegree;
						FQuat TotalRotation = FQuat::FindBetweenNormals(LastInputVector, ForwardVector);
						TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation.Inverse(), Alpha);
						LastInputVector = TotalRotation.RotateVector(ForwardVector);
					}
				}
			}
		}
	}

	Super::OnStateChanged(LastState);
}

FTransform URootMotionModifier_ATKRotation::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
	const ACharacter* CharacterOwner = GetCharacterOwner();
	if (CharacterOwner == nullptr)
	{
		return InRootMotion;
	}

	const UCharacterMovementComponent* CharacterMovement = CharacterOwner->GetCharacterMovement();
	if (CharacterMovement == nullptr)
	{
		return InRootMotion;
	}

	//const FVector LastInputVector = CharacterMovement->GetLastInputVector();
	if (LastInputVector.IsNearlyZero())
	{
		return InRootMotion;
	}

	FTransform FinalRootMotion = InRootMotion;

	const FTransform CharacterTransform = CharacterOwner->GetTransform();

	const FQuat CharacterQuat = CharacterTransform.GetRotation();
	const FQuat LastInputQuat = LastInputVector.ToOrientationQuat();

	const float TimeRemaining = (EndTime - PreviousPosition);
	const float Alpha = FMath::Clamp(DeltaSeconds / TimeRemaining, 0.f, 1.f);
	const FQuat TargetRotThisFrame = FQuat::Slerp(CharacterQuat, LastInputQuat, Alpha);

	const FQuat TargetQuat = TargetRotThisFrame * CharacterQuat.Inverse();
	const FQuat DeltaQuat = TargetQuat * InRootMotion.GetRotation();

	FinalRootMotion.SetRotation(DeltaQuat);

	if (bWarpTranslation)
	{
		const FTransform MeshRelativeTransform = FTransform(CharacterOwner->GetBaseRotationOffset(), CharacterOwner->GetBaseTranslationOffset());
		const FTransform MeshTransform = MeshRelativeTransform * CharacterOwner->GetActorTransform();

		const FVector DesiredTranslation = MeshTransform.TransformPosition(InRootMotion.GetTranslation());
		FVector BoneTranslation = GetBoneTransform(BoneSocketName).GetTranslation();
		BoneTranslation.Z = DesiredTranslation.Z;

		const FVector CurrentDirection = DesiredTranslation - BoneTranslation;
		const FVector DesiredDirection = TargetQuat.RotateVector(CurrentDirection);
		const FVector DestDesiredTranslation = BoneTranslation + DesiredDirection;
		const FVector DeltaTranlation = MeshTransform.InverseTransformPosition(DestDesiredTranslation);

		FinalRootMotion.SetTranslation(DeltaTranlation);
	}

	return FinalRootMotion;
}

FTransform URootMotionModifier_ATKRotation::GetBoneTransform(const FName& InBoneSocketName) const
{
	if (const ACharacter* CharacterOwner = GetCharacterOwner())
	{
		if (const USkeletalMeshComponent* CharacterMesh = CharacterOwner->GetMesh())
		{
			return InBoneSocketName.IsNone() ? CharacterOwner->GetTransform() : CharacterMesh->GetSocketTransform(InBoneSocketName);
		}
	}

	return FTransform::Identity;
}
