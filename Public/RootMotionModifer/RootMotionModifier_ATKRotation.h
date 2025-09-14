// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"
#include "RootMotionModifier_ATKRotation.generated.h"

/**
 根据玩家输入旋转角色
 */
UCLASS()
class PRACTICEDEMO_API URootMotionModifier_ATKRotation : public URootMotionModifier
{
	GENERATED_BODY()
public:
	URootMotionModifier_ATKRotation(const FObjectInitializer& ObjectInitializer);
public:
	/** If true, Degree threshold will be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bUseDegreeThreshold;
	//给AI用的让AI旋转朝向目标
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bForAI;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bForAI"))
	FName TargetKeyName;
	/** Degree threshold during rotation warp of the root motion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bUseDegreeThreshold", ClampMin = "0", UIMin = "0", ClampMax = "180", UIMax = "180"))
	float DegreeThreshold;

	/** Whether to warp the translation component of the root motion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	bool bWarpTranslation;

	/** The name of the bone that is being rotated around its axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bWarpTranslation"))
	FName BoneSocketName;

	

	/** Called when the state of the modifier changes. */
	virtual void OnStateChanged(ERootMotionModifierState LastState) override;

	/** Process RootMotion. */
	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;

	/** Get Bone Transform int world space. */
	FTransform GetBoneTransform(const FName& InBoneSocketName) const;
protected:

	UPROPERTY()
	FVector LastInputVector;
};
