// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorGameplayAbility.h"
#include<DebugHelper/Debug.h>

#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<Components/Combat/PawnCombatComponent.h>
#include<AbilitySystemBlueprintLibrary.h>
#include<WarriorFunctionLibrary.h>
#include<WarriorGameplayTags.h>
void UWarriorGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo && !Spec.IsActive())
		{
			ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle);
		}
	}
}

void UWarriorGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (AbilityActivationPolicy == EWarriorAbilityActivationPolicy::OnGiven)
	{
		if (ActorInfo)
		{
			ActorInfo->AbilitySystemComponent->ClearAbility(Handle);
			if (Handle == GetWarriorAbilitySystemComponentFromActorInfo()->GetCurrentActivatedAbility())
			{
				GetWarriorAbilitySystemComponentFromActorInfo()->SetCurrentActivatedAbility(FGameplayAbilitySpecHandle());
			}
		}
	}
} 

UPawnCombatComponent* UWarriorGameplayAbility::GetPawnCombatComponentFromActorInfo() const
{
	return GetAvatarActorFromActorInfo()->FindComponentByClass<UPawnCombatComponent>();
}

UWarriorAbilitySystemComponent* UWarriorGameplayAbility::GetWarriorAbilitySystemComponentFromActorInfo() const
{
	return Cast<UWarriorAbilitySystemComponent>(CurrentActorInfo->AbilitySystemComponent);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
	UAbilitySystemComponent*TargetASC= UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	checkf(TargetASC && InSpecHandle.IsValid(),TEXT("%s's UAbilitySystemComponent or FGameplayEffectSpecHandle is invalid in NativeApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle)"),*TargetActor->GetActorNameOrLabel());
	return GetWarriorAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(
		*InSpecHandle.Data,
		TargetASC
	);
}

FActiveGameplayEffectHandle UWarriorGameplayAbility::BP_ApplyEffectSpecHandleToTarget(AActor* TargetActor, const FGameplayEffectSpecHandle& InSpecHandle, EWarriorSuccessType& OutSuccessType)
{ 
	FActiveGameplayEffectHandle ActiveGameplayEffectHandle=NativeApplyEffectSpecHandleToTarget(TargetActor, InSpecHandle);
	OutSuccessType = ActiveGameplayEffectHandle.WasSuccessfullyApplied() ? EWarriorSuccessType::Successful : EWarriorSuccessType::Failed;
	return ActiveGameplayEffectHandle;
}

void UWarriorGameplayAbility::ApplyGameplayEffectSpecHandleToHitResults(const FGameplayEffectSpecHandle& InSpecHandle, const TArray<FHitResult>& InHitResults)
{
	if (InHitResults.IsEmpty())return;
	APawn* OwningPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	for (const auto& Hit : InHitResults)
	{
		if (APawn* HitPawn = Cast<APawn>(Hit.GetActor()))
		{
			if (UWarriorFunctionLibrary::IsTargetPawnHostile(OwningPawn, HitPawn))
			{
				FActiveGameplayEffectHandle ActiveGameplayEffectHandle=NativeApplyEffectSpecHandleToTarget(HitPawn, InSpecHandle);
				if (ActiveGameplayEffectHandle.WasSuccessfullyApplied())
				{
					FGameplayEventData Data;
					Data.Instigator = OwningPawn;
					Data.Target = HitPawn;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
						HitPawn,
						WarriorGameplayTags::Shared_Event_HitReact,
						Data
					);
				}
			}
		}
	}
}

FGameplayEffectSpecHandle UWarriorGameplayAbility::MakeHasDurationEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float DurationTime)
{
	check(EffectClass);
	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	UGameplayEffect*CDO=EffectClass.GetDefaultObject();
	CDO->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	CDO->DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(DurationTime));
	FGameplayEffectSpecHandle SpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
	return SpecHandle;
}

FGameplayEffectSpecHandle UWarriorGameplayAbility::MakeInstantCostEffectSpecHandle(TSubclassOf<UGameplayEffect>EffectClass,TArray<FGameplayModifierInfo> Modifiers)
{
	if(Modifiers.IsEmpty())return FGameplayEffectSpecHandle();
	FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
	UGameplayEffect*CDO=EffectClass.GetDefaultObject();
	TArray<FGameplayModifierInfo>InitModifier = CDO->Modifiers;
	CDO->Modifiers = Modifiers;
	FGameplayEffectSpecHandle SpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
		EffectClass,
		GetAbilityLevel(),
		ContextHandle
	);
	//CDO->Modifiers = InitModifier;
	return SpecHandle;
}

FGameplayModifierInfo UWarriorGameplayAbility::MakeGameplayModifierInfo(FGameplayAttribute Attribute, TEnumAsByte<EGameplayModOp::Type> ModifierOp, float ModifierMagnitude, FGameplayTagRequirements SourceTags, FGameplayTagRequirements TargetTags)
{
	FGameplayModifierInfo Info;
	Info.Attribute = Attribute;
	Info.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(ModifierMagnitude));
	Info.ModifierOp = ModifierOp;
	Info.SourceTags = SourceTags;
	Info.TargetTags = TargetTags;
	return Info;
}


