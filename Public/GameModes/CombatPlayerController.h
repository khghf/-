// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include<GenericTeamAgentInterface.h>
#include "CombatPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API ACombatPlayerController : public APlayerController,public IGenericTeamAgentInterface
{
	GENERATED_BODY()
public:
	ACombatPlayerController();
#pragma region Interfaces
	///IGenericTeamAgentInterface///
	virtual FGenericTeamId GetGenericTeamId() const override;
#pragma endregion
private:
	FGenericTeamId HeroTeamId;
};
