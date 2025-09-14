// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnExtensionComponentBase.h"
#include<GameplayTagContainer.h>
#include "TargetLockComponent.generated.h"
class UWarriorWidgetBase;
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API UTargetLockComponent : public UPawnExtensionComponentBase
{
	GENERATED_BODY()
public:
	UTargetLockComponent();
	virtual void BeginPlay()override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)override;
	UFUNCTION(BlueprintCallable)
	void ToggleTargetLock(bool bEnableTargetLock);
	//小于0向左切换，大于0右切换
	UFUNCTION(BlueprintCallable)
	void SwitchTarget(float value);
protected:
	void TryLockOnTarget();
	void TryLockOnTarget(AActor*LockTarget);
	void EndLock();
	AActor* FindActorToLock();
	FTimerHandle TickHandle;
	void OnTargetLockTick(float DeltaTime);
	void UpdateCameraRotation(float DeltaTime);
	void DrawTargetLockWidget();
	void SetTargetLockWidgetPosition();
	void CleanUp();
private:
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	bool bAutoLockOnNextTargetWhenCurrentTargetDead = false;
	//检测盒子大小
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	FVector TraceBoxSize = FVector(5000.f, 5000.f, 300.f);
	//检车通道
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	TArray<TEnumAsByte<	EObjectTypeQuery> >BoxTraceChannle;
	//Debug线条
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	bool bShowPersistentDebugShape = false;
	//提示控件
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	TSubclassOf<UWarriorWidgetBase>TargetLockWidgetClass;
	//相机自动调整速度
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	float TargetLockRotationInterSpeed = 5.f;
	//锁定时限制速度大小
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	bool bClampWalkSpeedWhenLocking = false;
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock", meta = (EditCondition = "bClampWalkSpeedWhenLocking"))
	float TargetLockMaxWalkSpeed = 200.f;
	//定时器(锁定目标期间会ick)Tick间隔
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock" )
	float TickTime = 0.2f;
	UPROPERTY()
	float CachedDefaultMaxWalkSpeed = 0.f;
	//相机位置偏移
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	FRotator TargetLockCameraOffset = FRotator(-20.f, 0.f, 0.f);
	UPROPERTY()
	AActor* CurrentLockedActor = nullptr;
	UPROPERTY()
	UWarriorWidgetBase* DrawnTargetLockWidget=nullptr;
	UPROPERTY()
	FVector2D TargetLockWidgetSize = FVector2D::ZeroVector;
	//提示控件位置偏移
	UPROPERTY(EditDefaultsOnly, Category = "TargetLock")
	FVector2D WidgetPositionOffset = FVector2D::ZeroVector;
	UPROPERTY()
	APlayerController* CachedPlayerController = nullptr;
	UPROPERTY()
	ACharacter* CachedCharacter = nullptr;
	UPROPERTY()
	FGameplayTag CachedGaitTag;
	UPROPERTY()
	FGameplayTag TargetLockGaitTag;
public:
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentLockTarget();
	bool GetbClampWalkSpeedWhenLocking()const;
};
