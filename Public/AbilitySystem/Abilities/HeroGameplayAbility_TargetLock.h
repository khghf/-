// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/WarriorHeroGameplayAbility.h"
#include "HeroGameplayAbility_TargetLock.generated.h"
class UWarriorWidgetBase;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UHeroGameplayAbility_TargetLock : public UWarriorHeroGameplayAbility
{
	GENERATED_BODY()
protected:
	//~Begin UGameAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
	//~End UGameAbility Interface
	UFUNCTION(BlueprintCallable)
	void OnTargetLockTick(float DeltaTime);
	UFUNCTION(BlueprintCallable)
	void SwitchTarget(const FGameplayTag& InSwitchDirectionTag);
private:
	void TryLockOnTarget();
	bool IsTryLockOnNextTargetWhenCurrentTargetWasDead();
	void GetAvailableActorsToLock();
	AActor* GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors);
	void GetAvailableActorsAroundTarget(TArray<AActor*>&OutActorsOnLeft, TArray<AActor*>&OutActorsOnRight);
	//锁定目标时绘制提醒控件
	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();
	//锁定目标时更新角色行走速度
	void InitTargetLockMovement();
	void InitTargetLockMappingContext();

	void CancelTargetLockAbility();
	void CleanUp();
	void ResetTargetLockMovement();
	void ResetTargetLockMappingContext();
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	bool bAutoLockOnNextTargetWhenCurrentTargetDead = false;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	float BoxTraceDistance = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	FVector TraceBoxSize = FVector(5000.f,5000.f,300.f);
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	TArray<TEnumAsByte<	EObjectTypeQuery> >BoxTraceChannle;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	bool bShowPersistentDebugShape = false;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	TSubclassOf<UWarriorWidgetBase>TargetLockWidgetClass;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	float TargetLockRotationInterSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	float TargetLockMaxWalkSpeed = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	UInputMappingContext* TargetLockMappingContext;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	FRotator TargetLockCameraOffset = FRotator(-20.f, 0.f, 0.f);
	UPROPERTY()
	TArray<AActor*>AvailableActorsToLock; 
	UPROPERTY()
	AActor* CurrentLockedActor;
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget;
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
};

