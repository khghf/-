// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include<WarriorTypes/WarriorStructTypes.h>
#include "WarriorEnemyLinkedAnimLayer.generated.h"
class UWarriorEnemyAnimInstance;
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UWarriorEnemyLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()

	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UWarriorEnemyAnimInstance* GetEnemyAnimInstance()const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations")
	FCardinalDirectionalAnim WalkCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations")
	FCardinalDirectionalAnim RunCardinal;
};
