// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include "WarriorCharacterAnimInstance.generated.h"

/**
 * 
 */
class ABaseCharacter;
class UCharacterMovementComponent;
class UAnimSequenceBase;


UCLASS()
class PRACTICEDEMO_API UWarriorCharacterAnimInstance : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation()override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds)override;
protected:
	UPROPERTY()
	ABaseCharacter* OwningCharacter;
	UPROPERTY()
	UCharacterMovementComponent* OwningMovementComponent;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Locomotion")
	float GroundSpeed;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Locomotion")
	bool bHasAcceleration;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Locomotion")
	float LocomotionDirection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|VelocityData")
	ECardinalDirection LocalVelocityDirection;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "AnimData|VelocityData")
	ECardinalDirection LocalVelocityDirectionNoOffset;
	
public:
	UFUNCTION(BlueprintCallable,BlueprintPure)
	ECardinalDirection SelectCardinalDirectionalFromAngle(const float& InAngle, const float& InDeadZone, const ECardinalDirection& InCurrentDirection, const bool& InbUseCurrentDirection);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	ECardinalDirection GetOppositeCaridinalDirection(const ECardinalDirection& InDirection);
};
