// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorCharacterAnimInstance.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include<Kismet/KismetMathLibrary.h>
#include "WarriorHeroAnimInstance.generated.h"

struct FFloatSpringState;
class AHeroCharacter;
UCLASS()
class PRACTICEDEMO_API UWarriorHeroAnimInstance : public UWarriorCharacterAnimInstance
{
	GENERATED_BODY()
public:
	UWarriorHeroAnimInstance();
	virtual void NativeInitializeAnimation()override;
	//virtual void NativeUpdateAnimation(float DeltaSeconds)override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds)override;
protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Refrences")
	AHeroCharacter* OwnningHeroCharacter;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Locomotion")
	bool bShouldEnterRelaxState;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Locomotion")
	float EnterRelaxStateThreshold = 5.f;
	float IdleElpasedTime; 
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocationData")
	FVector WorldLocation=FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocationData")
	float DisplacementSinceLastUpdate=0.f;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|LocationData")
	float DisplacementSpeed=0.f;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|RotationData")
	FRotator WorldRotation = FRotator::ZeroRotator;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|RotationData")
	float YawDeltaSinceLastUpdate = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	FVector WorldVelocity = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	FVector WorldVelocity2D = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	FVector LocalVelocity2D = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	bool bWasMovingLastUpdate = false;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	bool bHasVelocity = false;
	//角色速度相对其朝向的夹角
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	float LocalVelocityDirectionAngle = 0.f;
	//为原地转身准备，先占个位置
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|VelocityData")
	float LocalVelocityDirectionAngleWithOffset = 0.f;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Acceleration")
	FVector WorldAcceleration2D = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Acceleration")
	FVector LocalAcceleration2D = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Acceleration")
	FVector PivotDirection2D = FVector::ZeroVector;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|Acceleration")
	ECardinalDirection CardinalDirectionFromAcceleration;

	UPROPERTY(BlueprintReadOnly, Category = "AnimData|MovementState")
	bool  bIsOnGround = false;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|MovementState")
	bool  bIsFalling = false;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|MovementState")
	bool  bIsJumping = false;
	//为原地转身准备，先占个位置
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	bool bEnableRootYawOffset =true;
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	float RootYawOffset = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData|TurnInPlace")
	FFloatSpringState RootYawOffsetSpringState;
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	float TurnYawCurveValue;
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	ERootYawOffsetMode RootYawOffsetMode;
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	FVector2D RootYawOffsetAngleClamp;
	UPROPERTY(BlueprintReadWrite, Category = "AnimData|TurnInPlace")
	FVector2D RootYawOffsetAngleClampCrouched;
	UPROPERTY(BlueprintReadOnly, Category = "AnimData")
	float  GroundDistance = 0.f;
	UPROPERTY(BlueprintReadOnly, Category = "CharacterStateData")
	bool bIsRunningIntoWall = false;
	UPROPERTY(BlueprintReadWrite, Category = "Locomotion|Data")
	ECardinalDirection StartDirection;
private:
	bool bIsFirstUpdate =true;
	float DeltaTime;
protected:
	//void NativeUpdateLocationData();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateLocationData();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateRotationData();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateVelocityData();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateWallDetectionHeuristic();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateAccelerationData();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateCharacterMovementState();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateRootYawOffset();
	UFUNCTION(meta = (BlueprintThreadSafe = "true"), Category = "ThreadSafeFunc")
	void UpdateGroundDistance();
protected:
	UFUNCTION(BlueprintCallable, Category = "TurnInPlace",meta = (BlueprintThreadSafe = "true"))
	void SetRootYawOffset(const float& InRootYawOffset);
};
