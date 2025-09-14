// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/Input/WarriorPreInputBufferComponent.h"

#include<DebugHelper/Debug.h>

#include<Kismet/KismetSystemLibrary.h>
#include<Kismet/GameplayStatics.h>
void UWarriorPreInputBufferComponent::ConsumeBuffer(EWarriorSuccessType& OutSuccessType)
{
	bIsBufferOpen = false;
	if (!TagToConsume.IsValid())
	{
		OutSuccessType = EWarriorSuccessType::Failed;
		return;
	}
	OnBufferConsumed.ExecuteIfBound(TagToConsume);
	TagToConsume = FGameplayTag::EmptyTag;
	OutSuccessType = EWarriorSuccessType::Successful;
}



void UWarriorPreInputBufferComponent::OpenBuffer()
{
	bIsBufferOpen = true;
	ClearStoredInput();
	TagToConsume = FGameplayTag::EmptyTag;
}

void UWarriorPreInputBufferComponent::CloseBuffer()
{
	bIsBufferOpen = false;
	if (StoredInput.size() > 0)
	{
		TagToConsume = StoredInput.top().InputTag;
	}
	ClearStoredInput();
}
void UWarriorPreInputBufferComponent::StoreInput(const FGameplayTag& InInputTag)
{
	if (!bIsBufferOpen)return;
	if (int32* Priority = InputPriorityConfig.Find(InInputTag))
	{
		StoredInput.emplace(InInputTag, *Priority);
	}
}

bool UWarriorPreInputBufferComponent::BufferIsEmpty()const
{
	return StoredInput.empty();
}

bool UWarriorPreInputBufferComponent::TagToConsumeIsNone() const
{
	return !TagToConsume.IsValid();
}

void UWarriorPreInputBufferComponent::ClearStoredInput()
{
	std::priority_queue<FCacheUnit, std::vector<FCacheUnit>, FCacheUnit>ForClear;
	StoredInput.swap(ForClear);
}
