// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include"GameplayTagContainer.h "
#include "PawnCombatComponent.generated.h"

class AWarriorWeaponBase;
UENUM(BlueprintType)
enum class EToggleDamageType :uint8
{
	CurrentWeapon,//武器造成的伤害
	LeftHand,//左手
	RightHand,//右手
};
UCLASS()
class PRACTICEDEMO_API UPawnCombatComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
private:
	TMap<FGameplayTag, AWarriorWeaponBase*>CharacterCarriedWeaponMap;
public:
	UPROPERTY(BlueprintReadWrite, Category = "Warrior|Combat")
	FGameplayTag CurrentEquippedWeaponTag;
public:
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void RegisterSpwanedWeapon(FGameplayTag InWeaponTag, AWarriorWeaponBase* InWeapon,bool bRegisterAsEquippedWeapon=false);
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCarriedWeaponByTag(FGameplayTag InWeaponTag)const;
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	AWarriorWeaponBase* GetCharacterCurrentEquippedWeapon()const;
	UFUNCTION(BlueprintCallable, Category = "Warrior|Combat")
	void ToggleWeaponCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType= EToggleDamageType::CurrentWeapon);

#pragma region DeletageFunctions
	//绑定看 RegisterSpwanedWeapon
	virtual void OnHitTargetActor(AActor* HitActor);
	virtual void OnWeaponPulledFromTargetActor(AActor* InteractedActor);
#pragma endregion
protected:
	TArray<AActor*>OverlapedActors;
	void ToggleCurrentEquippedWeaponCollision(bool bShouldEnable);
	virtual void ToggleBodyCollisionBoxCollision(bool bShouldEnable, EToggleDamageType ToggleDamageType);
};
