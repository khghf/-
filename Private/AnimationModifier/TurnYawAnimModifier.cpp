// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationModifier/TurnYawAnimModifier.h"
#include<Editor/AnimationBlueprintLibrary/Public/AnimationBlueprintLibrary.h>
#include<Editor/AnimationBlueprintLibrary/Public/AnimPose.h>
#include<Kismet/KismetMathLibrary.h>

#include<DebugHelper/Debug.h>
void UTurnYawAnimModifier::OnApply_Implementation(UAnimSequence* AnimationSequence)
{
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, TurnYawCurveName);
	int32 FirstZeroTurnFrame = PopulateCurveKeys(AnimationSequence);
	UAnimationBlueprintLibrary::AddCurve(AnimationSequence, WeightCurveName);
	UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, WeightCurveName, 0.f,1.f);
	UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, WeightCurveName, UAnimationBlueprintLibrary::GetTimeAtFrameInternal(AnimationSequence, FirstZeroTurnFrame), 1.f);
	UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, WeightCurveName, UAnimationBlueprintLibrary::GetTimeAtFrameInternal(AnimationSequence, FirstZeroTurnFrame+1), 0.f);
}

void UTurnYawAnimModifier::OnRevert_Implementation(UAnimSequence* AnimationSequence)
{
	UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, TurnYawCurveName);
	UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, WeightCurveName);
}

int32 UTurnYawAnimModifier::PopulateCurveKeys(UAnimSequence* AnimationSequence)
{
	int32 NumFrames = 0;
	int32 FirstZeroTurnFrame = -1;
	FAnimPoseEvaluationOptions PoseEvaluationOptions;
	UAnimationBlueprintLibrary::GetNumFrames(AnimationSequence, NumFrames);
	FAnimPose Pose;
	UAnimPoseExtensions::GetAnimPoseAtFrame(AnimationSequence, NumFrames, PoseEvaluationOptions, Pose);
	const float&TotalTurnYaw=UAnimPoseExtensions::GetBonePose(Pose, RootBoneName).Rotator().Yaw;
	for (int i = 0; i < NumFrames; i++)
	{
		UAnimPoseExtensions::GetAnimPoseAtFrame(AnimationSequence, i, PoseEvaluationOptions, Pose);
		const float&CurrentPoseTurnYaw= UAnimPoseExtensions::GetBonePose(Pose, RootBoneName).Rotator().Yaw;
		UAnimationBlueprintLibrary::AddFloatCurveKey(AnimationSequence, TurnYawCurveName, UAnimationBlueprintLibrary::GetTimeAtFrameInternal(AnimationSequence, i), TotalTurnYaw - CurrentPoseTurnYaw);
		if (UKismetMathLibrary::NearlyEqual_FloatFloat(TotalTurnYaw - CurrentPoseTurnYaw, 0.f, ErrorTolerance) && FirstZeroTurnFrame == -1)
		{
			FirstZeroTurnFrame = i;
		}
		/*FString text = FString::Printf(TEXT("%f,FirstZeroTurnFrame:%d"), TotalTurnYaw - CurrentPoseTurnYaw, FirstZeroTurnFrame);
		Debug::Print(text);*/
	}
	return UKismetMathLibrary::Clamp(FirstZeroTurnFrame + FirstZeroTurnFrameOffset, 0, NumFrames);
}
