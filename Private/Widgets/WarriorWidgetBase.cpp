// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/WarriorWidgetBase.h"


#include<Interfaces/PawnUIInterface.h>
#include<Components/UI/PawnUIComponent.h>
#include<Components/UI/HeroUIComponent.h>
#include<Components/UI/EnemyUIComponent.h>
void UWarriorWidgetBase::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(GetOwningPlayerPawn()))
	{
		if (UHeroUIComponent* HeroUIComponent = PawnUIInterface->GetHeroUIComponent())
		{
			BP_OnOwningHeroUIComponentInitialized(HeroUIComponent);
		}
	}
}


void UWarriorWidgetBase::InitEnemyCreatedWidget(AActor* OwningEnemyActor)
{
	if (IPawnUIInterface* PawnUIInterface = Cast<IPawnUIInterface>(OwningEnemyActor))
	{
		UEnemyUIComponent* EnemyUIComponent = PawnUIInterface->GetEnemyUIComponent();
		checkf(EnemyUIComponent, TEXT("Failed to extrac 'UEnemyUIComponent' from %s") ,* OwningEnemyActor->GetActorNameOrLabel());
		BP_OnOwningEnemyUIComponentInitialized(EnemyUIComponent);
	}
}
