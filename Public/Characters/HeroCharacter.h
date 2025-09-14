// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include<GameplayTagContainer.h>
#include "HeroCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UDataAsset_InputConfig;
struct FInputActionValue;
class UHeroCombatComponent;
class UHeroUIComponent;
class UWarriorPreInputBufferComponent;
class UTargetLockComponent;
UCLASS()
class PRACTICEDEMO_API AHeroCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	AHeroCharacter();
public:
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)override;
	//~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	//~ End APawn Interface
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultWalkSpeed = 225.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultRunSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultSprintSpeed = 500.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
	float DefaultCrouchWalkSpeed = 200.f;
private:
#pragma region Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FllowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	UHeroCombatComponent* HeroCombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI", meta = (AllowPrivateAccess = "true"))
	UHeroUIComponent* HeroUIComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UWarriorPreInputBufferComponent* PreInputBufferCom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TargetLock", meta = (AllowPrivateAccess = "true"))
	UTargetLockComponent* TargetLocakCom;
#pragma endregion

#pragma region Inputs
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;
	void IA_Move_Func(const FInputActionValue& Value);
	void IA_Look_Func(const FInputActionValue& Value);
	void IA_SwitchBetweenWalkAndRun_Func(const FInputActionValue& Value);

	void IA_SwitchTargetTriggered_Func(const FInputActionValue& Value);
	void IA_SwitchTargetCompleted_Func(const FInputActionValue& Value);

	void IA_PickUpStonesStarted_Func(const FInputActionValue& Value);

	FVector2D SwitchDirection = FVector2D::ZeroVector;
	void Input_AbilityInputPressed(FGameplayTag InputTag);
	void Input_AbilityInputReleased(FGameplayTag InputTag);

#pragma endregion
public:
#pragma region NativeFunctions
	FORCEINLINE UHeroCombatComponent* GetHeroCombatComponent()const { return HeroCombatComponent; }
#pragma endregion
	
	
#pragma region IPawnCombatInterface
	virtual UPawnCombatComponent* GetPawnCombatComponent()const override;
	virtual UHeroUIComponent* GetHeroUIComponent()const override;
#pragma endregion

#pragma region IPawnUIInterface
	virtual UPawnUIComponent* GetPawnUIComponent()  const override;
#pragma endregion
};
