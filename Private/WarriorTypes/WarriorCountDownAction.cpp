// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorTypes/WarriorCountDownAction.h"

#include<DebugHelper/Debug.h>
void FWarriorCountDownAction::UpdateOperation(FLatentResponse& Response)
{
	if (bNeedToCancel)
	{
		CountOutput = EWarriorCountDownActionOutInput::Cancelled;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}
	if (ElapsedTimeSinceStart >= TotalCountTime|| bRefreshNow)
	{
		CountOutput = EWarriorCountDownActionOutInput::Completed;
		Response.FinishAndTriggerIf(true, ExecutionFunction, OutputLink, CallbackTarget);
		return;
	}
	if (ElapsedInterval < UpdateInterval)
	{
		ElapsedInterval += Response.ElapsedTime();
	}
	else
	{
		ElapsedTimeSinceStart += UpdateInterval > 0.f ? UpdateInterval : Response.ElapsedTime();
		OutRemainingTime = TotalCountTime - ElapsedTimeSinceStart;
		CountOutput = EWarriorCountDownActionOutInput::Update;
		Response.TriggerLink(ExecutionFunction, OutputLink, CallbackTarget);
		ElapsedInterval = 0.f;
	}
}

void FWarriorCountDownAction::NotifyObjectDestroyed()
{
	//Debug::Print("FWarriorCountDownAction::NotifyObjectDestroyed");
}

void FWarriorCountDownAction::NotifyActionAborted()
{
	//Debug::Print("FWarriorCountDownAction::NotifyActionAborted");

}

void FWarriorCountDownAction::CancelAction()
{
	bNeedToCancel = true;
}

void FWarriorCountDownAction::RefreshNow()
{
	bRefreshNow = true;
}
