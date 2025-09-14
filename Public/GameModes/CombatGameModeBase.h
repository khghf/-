// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include "CombatGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API ACombatGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	ACombatGameModeBase();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameSettings")
	EWarriorGameDifficulty CurrentGameDifficulty; 
public:
	FORCEINLINE EWarriorGameDifficulty GetCurrentGameDifficulty()const { return CurrentGameDifficulty; }
};
