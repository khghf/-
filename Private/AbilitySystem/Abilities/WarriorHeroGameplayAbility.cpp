// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include<Characters/HeroCharacter.h>
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include "WarriorGameplayTags.h"
AHeroCharacter* UWarriorHeroGameplayAbility::GetHeroCharacterFromActorInfo()
{
    if (!CachedHeroCharacter.IsValid())
    {
        CachedHeroCharacter = Cast<AHeroCharacter>(CurrentActorInfo->AvatarActor);
    }
    return CachedHeroCharacter.IsValid()? CachedHeroCharacter.Get():nullptr;
}

ACombatPlayerController* UWarriorHeroGameplayAbility::GetCombatPlayerControllerFromActorInfo()
{
    if (!CachedCombatPlayerController.IsValid())
    {
        CachedCombatPlayerController = Cast<ACombatPlayerController>(CurrentActorInfo->PlayerController);
    }
    return CachedCombatPlayerController.IsValid() ? CachedCombatPlayerController.Get() : nullptr;
}

UHeroCombatComponent* UWarriorHeroGameplayAbility::GetHeroCombatComponentFromActorInfo()
{
    return GetHeroCharacterFromActorInfo()->GetHeroCombatComponent();
}

UHeroUIComponent* UWarriorHeroGameplayAbility::GetHeroUIComponentFromActorInfo()
{
    return GetHeroCharacterFromActorInfo()->GetHeroUIComponent();
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroDamageEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, float InWeaponBaseDamage, FGameplayTag InCurrentAttackTypeTag, int32 InUsedComboCount)
{
    check(EffectClass);
    FGameplayEffectContextHandle ContextHandle=GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
    );
    EffectSpecHandle.Data->SetSetByCallerMagnitude(
    WarriorGameplayTags::Shared_SetByCaller_BaseDamage,
        InWeaponBaseDamage
    );

    if (InCurrentAttackTypeTag.IsValid())
    {
        EffectSpecHandle.Data->SetSetByCallerMagnitude(
            InCurrentAttackTypeTag,
            InUsedComboCount
        );
    }
    return EffectSpecHandle;
}

FGameplayEffectSpecHandle UWarriorHeroGameplayAbility::MakeHeroCostEffectSpecHandle(TSubclassOf<UGameplayEffect> EffectClass, AActor* Instigator, AActor* Causer)
{
    check(EffectClass);
    FGameplayEffectContextHandle ContextHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeEffectContext();
    ContextHandle.SetAbility(this);
    ContextHandle.AddSourceObject(GetAvatarActorFromActorInfo());
    ContextHandle.AddInstigator(Instigator, Causer);
    FGameplayEffectSpecHandle EffectSpecHandle = GetWarriorAbilitySystemComponentFromActorInfo()->MakeOutgoingSpec(
        EffectClass,
        GetAbilityLevel(),
        ContextHandle
        );
    return EffectSpecHandle;
}

bool UWarriorHeroGameplayAbility::GetAbilityRemainingCooldownByTag(FGameplayTag InCooldownTag, float& TotalCooldownTime, float& RemainingCooldownTime)
{
    check(InCooldownTag.IsValid());
    FGameplayEffectQuery CooldownQuery=FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(InCooldownTag.GetSingleTagContainer());
    TArray<TPair<float,float>>TimeRemainingAndDuration=GetAbilitySystemComponentFromActorInfo()->GetActiveEffectsTimeRemainingAndDuration(CooldownQuery);
    if (!TimeRemainingAndDuration.IsEmpty())
    {
        RemainingCooldownTime = TimeRemainingAndDuration[0].Key;
        TotalCooldownTime = TimeRemainingAndDuration[0].Value;
    }
    return RemainingCooldownTime>0.f;
}
