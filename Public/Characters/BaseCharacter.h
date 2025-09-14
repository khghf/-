// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include<AbilitySystemInterface.h>
#include "Interfaces/PawnCombatInterface.h"
#include "Interfaces/PawnUIInterface.h"
#include "BaseCharacter.generated.h"
class UWarriorAbilitySystemComponent;
class UWarriorAttributeSet;
class UDataAsset_StartUpDataBase;
class UMotionWarpingComponent;
UCLASS()
class PRACTICEDEMO_API ABaseCharacter : public ACharacter ,public IAbilitySystemInterface, public IPawnCombatInterface,public IPawnUIInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
protected:
#pragma region AbilitySystem
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAbilitySystemComponent* WarriorAbilitySystemComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AbilitySystem")
	UWarriorAttributeSet* WarriorAttributeSet;
	
#pragma endregion
#pragma region DataAsset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData")
	TSoftObjectPtr<UDataAsset_StartUpDataBase>CharacterStartUpdata;
#pragma endregion
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "MotionWarping")
	UMotionWarpingComponent* MotionWarpingComponent;

	
public:
#pragma region APawn
	virtual void PossessedBy(AController* NewController) override;
#pragma endregion

#pragma region IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent()const override;
	virtual UAttributeSet* GetAttributeSet() const override;
#pragma endregion

#pragma region IPawnCombatInterface
	virtual UPawnCombatComponent* GetPawnCombatComponent()const override;
#pragma endregion

#pragma region IPawnUIInterface
	virtual UPawnUIComponent* GetPawnUIComponent()  const override;
#pragma endregion
	FORCEINLINE UWarriorAbilitySystemComponent* GetWarriorAbilitySystemComponent()const { return WarriorAbilitySystemComponent; }
	FORCEINLINE UWarriorAttributeSet* GetWarriorAttributeSet()const { return WarriorAttributeSet; }
};
