// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
namespace Debug
{
	static void Print(const FString& Msg, const float& DisplayTime = 5.f, const FColor& Color = FColor::MakeRandomColor(), const int32& Key=-1)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(Key, DisplayTime, Color, Msg);
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		}
	}
}
