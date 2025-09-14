// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/WarriorAttributeSet.h"


#include<GameplayEffectExtension.h>
#include<WarriorGameplayTags.h>
#include<WarriorFunctionLibrary.h>
#include<Components/UI/PawnUIComponent.h>
#include<Components/UI/HeroUIComponent.h>
#include<Interfaces/PawnUIInterface.h>
UWarriorAttributeSet::UWarriorAttributeSet()
{
	InitCurrentHealth(1.f);		InitMaxHealth(1.f);
	InitCurrentRage(1.f);		InitMaxRage(1.f);
	InitCurrentSTA(1.f);		InitMaxSTA(1.f);
	InitAttackPower(1.f);
	InitDefensePower(1.f);
	InitWeight(1.f);
	InitSTACost_Block(10.f);
	InitSTACost_Coefficient(1.f);
}

void UWarriorAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	if (!CachedPawnUIInterface.IsValid())
	{
		CachedPawnUIInterface = TWeakInterfacePtr<IPawnUIInterface>(Data.Target.GetAvatarActor());
	}
	checkf(CachedPawnUIInterface.IsValid(), TEXT("%s didn't inplement 'IPawnUIInterface'"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	UPawnUIComponent* PawnUIComponent = CachedPawnUIInterface->GetPawnUIComponent();
	checkf(PawnUIComponent, TEXT("Couldn't extrac a 'PawnUIComponent' from %s"), *Data.Target.GetAvatarActor()->GetActorNameOrLabel());
	
	if (Data.EvaluatedData.Attribute == GetCurrentHealthAttribute())
	{
		const float& NewValue = FMath::Clamp(GetCurrentHealth(), 0.f, GetMaxHealth());
		SetCurrentHealth(NewValue);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth()/ GetMaxHealth());
	}
	if (Data.EvaluatedData.Attribute == GetCurrentRageAttribute())
	{
		const float& NewValue = FMath::Clamp(GetCurrentRage(), 0.f, GetMaxRage());
		SetCurrentRage(NewValue);
		if (GetCurrentRage() == GetMaxRage())
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
		}
		else if (GetCurrentRage() == 0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}
		else
		{
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_Full);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(Data.Target.GetAvatarActor(), WarriorGameplayTags::Player_Status_Rage_None);
		}
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentRageChanged.Broadcast(GetCurrentRage() / GetMaxRage());
		}
	}
	//ÊÜÉË¿ÛÑª
	if (Data.EvaluatedData.Attribute == GetDamageTakenAttribute())
	{
		const float& OldHealth = GetCurrentHealth();
		const float& DamageDone = GetDamageTaken();
		const float& NewValue = FMath::Clamp(OldHealth- DamageDone, 0.f, GetMaxHealth());
		SetCurrentHealth(NewValue);
		PawnUIComponent->OnCurrentHealthChanged.Broadcast(GetCurrentHealth() / GetMaxHealth());
		if (NewValue == 0.f)
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(Data.Target.GetAvatarActor(),WarriorGameplayTags::Shared_Status_Dead);
		}
	}
	if (Data.EvaluatedData.Attribute == GetCurrentSTAAttribute())
	{
		
		const float& NewValue = FMath::Clamp(GetCurrentSTA(), 0.f, GetMaxSTA());
		SetCurrentSTA(NewValue);
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentSTAChanged.Broadcast(GetCurrentSTA() / GetMaxSTA());
		}
	}
	//¸ñµ²¼õSTA;
	if (Data.EvaluatedData.Attribute == GetSTACost_CoefficientAttribute())
	{
		const float& OldSTA = GetCurrentSTA();
		const float& Coefficient = FMath::Clamp(GetSTACost_Coefficient(), 0.5f,4.f);
		const float& STACost = Coefficient * GetSTACost_Block();
		const float& NewValue = FMath::Clamp(OldSTA - STACost, 0.f, GetMaxSTA());
		SetCurrentSTA(NewValue);
		if (UHeroUIComponent* HeroUIComponent = CachedPawnUIInterface->GetHeroUIComponent())
		{
			HeroUIComponent->OnCurrentSTAChanged.Broadcast(GetCurrentSTA() / GetMaxSTA());
		}
	}
}
