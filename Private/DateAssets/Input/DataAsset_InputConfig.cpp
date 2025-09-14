// Fill out your copyright notice in the Description page of Project Settings.


#include "DateAssets/Input/DataAsset_InputConfig.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag)const
{
	for (const FWarriorInputActionConfig& InputActionConfig : NativeInputAction)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}
