// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include<GameplayTagContainer.h>
#include<ScalableFloat.h>
#include "WarriorStructTypes.generated.h"

class UWarriorHeroLinkedAnimLayer;
class UWarriorGameplayAbility;
class UWarriorHeroGameplayAbility;
class UInputMappingContext;
USTRUCT(BlueprintType)
struct FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UWarriorHeroGameplayAbility>AbilityToGrant;
	bool IsValid()const;
};
USTRUCT(BlueprintType)
struct FWarriorHeroSpecialAbilitySet:public FWarriorHeroAbilitySet
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UMaterialInterface>SoftAbilityIconMaterial;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "Player.Cooldown"))
	FGameplayTag AbilityCooldownTag;
};
USTRUCT(BlueprintType)
struct FWarriorHeroWeaponData
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<UWarriorHeroLinkedAnimLayer>WeaponAnimLayerToLink;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	UInputMappingContext* WeaponInputMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty="InputTag"))
	TArray<FWarriorHeroAbilitySet>DefaultWeaponAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (TitleProperty = "InputTag"))
	TArray<FWarriorHeroSpecialAbilitySet>SpecialWeaponAbilities;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FScalableFloat WeaponBaseDamage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D>SoftWeaponIconTexture;
};
//动画的结构体
USTRUCT(BlueprintType)
struct FCardinalDirectionalAnim
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequenceBase* Forward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequenceBase* Backward;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequenceBase* Left;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UAnimSequenceBase* Right;
};
