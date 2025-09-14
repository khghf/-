// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CombatGameModeBase.h"

ACombatGameModeBase::ACombatGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}
