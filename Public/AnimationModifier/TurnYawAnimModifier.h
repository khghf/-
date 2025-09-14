// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "TurnYawAnimModifier.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UTurnYawAnimModifier : public UAnimationModifier
{
	GENERATED_BODY()
public:
	virtual void OnApply_Implementation(UAnimSequence* AnimationSequence)override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimationSequence)override;
public:
	UPROPERTY(EditInstanceOnly)
	FName RootBoneName = FName("root");
	UPROPERTY(EditInstanceOnly)
	FName TurnYawCurveName=FName("RemainingTurnYaw");
	UPROPERTY(EditInstanceOnly)
	FName WeightCurveName=FName("TurnYawWeight");
	//偏移权重为0的帧的位置，<0向左偏移，>0向右偏移
	UPROPERTY(EditInstanceOnly)
	int32 FirstZeroTurnFrameOffset = 0;
	//判断剩余转动角度是否为0时的容许误差
	UPROPERTY(EditInstanceOnly)
	float ErrorTolerance = 0.12f;
protected:
	int32 PopulateCurveKeys(UAnimSequence* AnimationSequence);
};
