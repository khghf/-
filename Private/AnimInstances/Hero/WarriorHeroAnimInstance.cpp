// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Hero/WarriorHeroAnimInstance.h"

#include<Kismet/KismetMathLibrary.h>
#include<Characters/HeroCharacter.h>
#include<GameFramework/CharacterMovementComponent.h>
#include<WarriorGameplayTags.h>
#include<WarriorFunctionLibrary.h>
UWarriorHeroAnimInstance::UWarriorHeroAnimInstance()
{
	bUseMultiThreadedAnimationUpdate = true;
}

void UWarriorHeroAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	if (OwningCharacter)
	{
		OwnningHeroCharacter = Cast<AHeroCharacter>(OwningCharacter);
	}
}
void UWarriorHeroAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	if (!OwnningHeroCharacter||!OwningMovementComponent)return;
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
	UpdateAccelerationData();
	UpdateWallDetectionHeuristic();
	UpdateCharacterMovementState();
	//UpdateRootYawOffset();
	UpdateGroundDistance();
	bIsFirstUpdate = false;
}



void UWarriorHeroAnimInstance::UpdateLocationData()
{

	if (bIsFirstUpdate)return;
	DisplacementSinceLastUpdate = (OwnningHeroCharacter->GetActorLocation() - WorldLocation).Size2D();
	WorldLocation = OwnningHeroCharacter->GetActorLocation();
	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate, DeltaTime);
}

void UWarriorHeroAnimInstance::UpdateRotationData()
{
	YawDeltaSinceLastUpdate = (OwningCharacter->GetActorRotation() - WorldRotation).Yaw;
	WorldRotation = OwningCharacter->GetActorRotation();
	if (bIsFirstUpdate)
	{
		YawDeltaSinceLastUpdate = 0.f;
	}
	
}

void UWarriorHeroAnimInstance::UpdateVelocityData()
{
	bWasMovingLastUpdate = !LocalVelocity2D.IsNearlyZero();
	WorldVelocity = OwningMovementComponent->Velocity;
	WorldVelocity2D = FVector(WorldVelocity.X, WorldVelocity.Y, 0.f);
	LocalVelocity2D = UKismetMathLibrary::Quat_UnrotateVector(WorldRotation.Quaternion(), WorldVelocity2D);
	bHasVelocity = UKismetMathLibrary::NearlyEqual_FloatFloat(LocalVelocity2D.Size2D(), 0.f);
	LocalVelocityDirectionAngle = UKismetMathLibrary::NormalizedDeltaRotator(WorldVelocity.Rotation(), WorldRotation).Yaw;
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	LocalVelocityDirection = SelectCardinalDirectionalFromAngle(LocalVelocityDirectionAngleWithOffset, 10.f, LocalVelocityDirection, bWasMovingLastUpdate);
	LocalVelocityDirectionNoOffset = SelectCardinalDirectionalFromAngle(LocalVelocityDirectionAngle, 10.f, LocalVelocityDirectionNoOffset, bWasMovingLastUpdate);
}

void UWarriorHeroAnimInstance::UpdateAccelerationData()
{
	WorldAcceleration2D = OwningMovementComponent->GetCurrentAcceleration() * FVector(1.f, 1.f, 0.f);
	LocalAcceleration2D = UKismetMathLibrary::Quat_UnrotateVector(WorldRotation.Quaternion(), WorldAcceleration2D);
	bHasAcceleration = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAcceleration2D.Size2D(), 0.f);
	//���ڼ��ٶȼ���һ������Pivot�˶�����Ҫ����
	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D, WorldAcceleration2D.GetSafeNormal(), 0.5f).GetSafeNormal();
	const float PivotAngle = UKismetMathLibrary::NormalizedDeltaRotator(PivotDirection2D.Rotation(), WorldRotation).Yaw;
	CardinalDirectionFromAcceleration = GetOppositeCaridinalDirection(SelectCardinalDirectionalFromAngle(PivotAngle, 10.f, ECardinalDirection::Forward, false));
}

void UWarriorHeroAnimInstance::UpdateWallDetectionHeuristic()
{
	//���߼�ͨ������ٶȺͼ��ٶ�֮���Ƿ��кܴ�ĽǶȣ�����ɫ������ǽ���ƣ���ʵ���ϻ���һ�ࣩ
	// �Լ���ɫ�Ƿ���ͼ���ٵ��ٶ���Խϵ����²��ɫ�Ƿ�ײ����ǽ�ϡ�
	bIsRunningIntoWall = LocalAcceleration2D.Size2D() > 0.1f;
	bIsRunningIntoWall = bIsRunningIntoWall && LocalVelocity2D.Size2D() < 200.f;
	bIsRunningIntoWall = bIsRunningIntoWall &&
		UKismetMathLibrary::InRange_FloatFloat(
			FVector::DotProduct(LocalAcceleration2D.GetSafeNormal(), LocalVelocity2D.GetSafeNormal()),
			-0.6f, 0.6f
		);
}

void UWarriorHeroAnimInstance::UpdateCharacterMovementState()
{
	bIsOnGround=OwningMovementComponent->IsMovingOnGround();
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

void UWarriorHeroAnimInstance::UpdateRootYawOffset()
{
	//���Ų��ƶ�ʱ�����磬�� Idle �ڼ䣩��������ƫ�Ƶ��� Pawn ��������ת�෴�ķ����Է�ֹ�������� Pawn һ����ת��
	if (RootYawOffsetMode == ERootYawOffsetMode::Accumulate)
	{
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
	}
	//�˶�ʱ��ƽ���ػ��ƫ�ơ�
	if (RootYawOffsetMode == ERootYawOffsetMode::BlendOut)
	{
		SetRootYawOffset(UKismetMathLibrary::FloatSpringInterp(RootYawOffset,0.f,RootYawOffsetSpringState,80.f,1.f,DeltaTime,1.f,0.5f));
	}
	//����ΪBlendOut��ÿ�θ��£�״̬����Ҫ�����ۻ�����ƫ����������ƫ�ƽ���ϡ�
	//����Ҫ����Ϊ�����״̬��ϣ�����ƫ�ƣ��������Ͳ���Ҫ���ÿ��״̬��
	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}

void UWarriorHeroAnimInstance::UpdateGroundDistance()
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

void UWarriorHeroAnimInstance::SetRootYawOffset(const float& InRootYawOffset)
{
	if (bEnableRootYawOffset)
	{
		float NewOffset = UKismetMathLibrary::NormalizeAxis(InRootYawOffset);
		if (RootYawOffsetAngleClamp.X != RootYawOffsetAngleClamp.Y)
		{
			RootYawOffset = UKismetMathLibrary::ClampAngle(NewOffset, RootYawOffsetAngleClamp.X, RootYawOffsetAngleClamp.Y);
		}
	}
	else
	{
		RootYawOffset = 0.f;
	}
}








