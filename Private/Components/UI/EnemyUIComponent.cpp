// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/EnemyUIComponent.h"

#include<Widgets/WarriorWidgetBase.h>
void UEnemyUIComponent::RegisterEnemyDrawnWidget(UWarriorWidgetBase* InWidgetToRegister)
{
	EnemyDrawnWidgets.Add(InWidgetToRegister);
}

void UEnemyUIComponent::RemoveEnemyDrawnWidgets()
{
	if (EnemyDrawnWidgets.IsEmpty())return;
	for (const auto& DrawnWidget : EnemyDrawnWidgets)
	{
		DrawnWidget->RemoveFromParent();
	}
	EnemyDrawnWidgets.Empty();
}
