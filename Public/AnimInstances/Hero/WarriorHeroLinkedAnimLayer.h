// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimInstances/WarriorBaseAnimInstance.h"
#include<WarriorTypes/WarriorStructTypes.h>
#include "WarriorHeroLinkedAnimLayer.generated.h"

class UWarriorHeroAnimInstance;
class UAnimSequenceBase;
class UCharacterMovementComponent;
UCLASS()
class PRACTICEDEMO_API UWarriorHeroLinkedAnimLayer : public UWarriorBaseAnimInstance
{
	GENERATED_BODY()
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UWarriorHeroAnimInstance* GetHeroAnimInstance()const;
	UFUNCTION(BlueprintPure, meta = (BlueprintThreadSafe))
	UCharacterMovementComponent* GetCharacterMovementComponent()const;
protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Walk")
	FCardinalDirectionalAnim WalkStartCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Walk")
	FCardinalDirectionalAnim WalkLoopCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Walk")
	FCardinalDirectionalAnim WalkStopCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Run")
	FCardinalDirectionalAnim RunStartCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Run")
	FCardinalDirectionalAnim RunLoopCardinal;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AnimData|Animations|Run")
	FCardinalDirectionalAnim RunStopCardinal;
};
