// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CombatPlayerController.h"

ACombatPlayerController::ACombatPlayerController()
{
    HeroTeamId = FGenericTeamId(0);
}

FGenericTeamId ACombatPlayerController::GetGenericTeamId() const
{
    return HeroTeamId;
}
