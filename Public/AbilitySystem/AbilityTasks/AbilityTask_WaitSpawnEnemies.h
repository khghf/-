// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitSpawnEnemies.generated.h"
class AEnemyCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWaitSpawnEnemiesDelegate, const TArray<AEnemyCharacter*>&, SpawnedEnemies);
UCLASS()
class PRACTICEDEMO_API UAbilityTask_WaitSpawnEnemies : public UAbilityTask
{ 
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Warrior|AbilityTasks", meta = (DisplayName = "Wait Gameplay Event And Spawn Enemies", HidePin = "OwningAbility",DefaultToSelf="OwningAbility",BlueprintInternalUseOnly="true", NumToSpawn="1", RandomSpawnRadius="200", SpwanLocationZOffset="150"))
	static UAbilityTask_WaitSpawnEnemies* WaitSpawnEnemies(
		UGameplayAbility*OwningAbility,
		FGameplayTag EventTag,
		TSoftClassPtr<AEnemyCharacter>SoftEnemyClassToSpawn,
		int32 NumToSpawn,
		const FVector&SpawnOrigin,
		float RandomSpawnRadius,
		float SpwanLocationZOffset
		);
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate OnSpawnFinished;
	UPROPERTY(BlueprintAssignable)
	FWaitSpawnEnemiesDelegate DidNotSpawn;

protected:
	//~Begin UGameplayTask Interface
	virtual void Activate()override;
	virtual void OnDestroy(bool bInOwnerFinished)override;
	//~End UGameplayTask Interface
private:
	FGameplayTag CachedEventTag;
	TSoftClassPtr<AEnemyCharacter>CachedSoftEnemyClassToSpawn;
	int32 CachedNumToSpawn;
	FVector CachedSpawnOrigin;
	float CachedRandomSpawnRadius;
	float CachedSpwanLocationZOffset;
	void OnGameplayEventReceived(const FGameplayEventData*InPayload);
	FDelegateHandle DelegateHandle;
	void OnEmemyClassLoaded();
};
