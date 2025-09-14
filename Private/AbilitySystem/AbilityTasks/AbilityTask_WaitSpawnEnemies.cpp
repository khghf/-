// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/AbilityTask_WaitSpawnEnemies.h"

#include<AbilitySystemComponent.h>
#include<Engine/AssetManager.h>
#include<NavigationSystem.h>
#include<Characters/EnemyCharacter.h>
UAbilityTask_WaitSpawnEnemies* UAbilityTask_WaitSpawnEnemies::WaitSpawnEnemies(UGameplayAbility* OwningAbility, FGameplayTag EventTag, TSoftClassPtr<AEnemyCharacter> SoftEnemyClassToSpawn, int32 NumToSpawn, const FVector& SpawnOrigin, float RandomSpawnRadius, float SpwanLocationZOffset)
{
	UAbilityTask_WaitSpawnEnemies*Node=NewAbilityTask<UAbilityTask_WaitSpawnEnemies>(OwningAbility);
	Node->CachedEventTag = EventTag;
	Node->CachedNumToSpawn = NumToSpawn;
	Node->CachedRandomSpawnRadius = RandomSpawnRadius;
	Node->CachedSoftEnemyClassToSpawn = SoftEnemyClassToSpawn;
	Node->CachedSpawnOrigin = SpawnOrigin;
	Node->CachedSpwanLocationZOffset = SpwanLocationZOffset;
	return Node;
}

void UAbilityTask_WaitSpawnEnemies::Activate()
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	DelegateHandle=Delegate.AddUObject(this, &ThisClass::OnGameplayEventReceived);
}

void UAbilityTask_WaitSpawnEnemies::OnDestroy(bool bInOwnerFinished)
{
	FGameplayEventMulticastDelegate& Delegate = AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(CachedEventTag);
	Delegate.Remove(DelegateHandle);
	Super::OnDestroy(bInOwnerFinished);
}

void UAbilityTask_WaitSpawnEnemies::OnGameplayEventReceived(const FGameplayEventData* InPayload)
{
	//确保并加载类软引用
	if (ensure(!CachedSoftEnemyClassToSpawn.IsNull()))
	{
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(
			CachedSoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateUObject(this, &ThisClass::OnEmemyClassLoaded)
		);
	}
	else
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AEnemyCharacter*>());
		}
		EndTask();
	}
	
}
//主要生成敌人逻辑
void UAbilityTask_WaitSpawnEnemies::OnEmemyClassLoaded()
{
	UClass* LoadedClass = CachedSoftEnemyClassToSpawn.Get();
	UWorld* World = GetWorld();
	if (!LoadedClass || !World)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			DidNotSpawn.Broadcast(TArray<AEnemyCharacter*>());
		}
		EndTask();
		return;
	}
	TArray<AEnemyCharacter*>SpawnwdEnemies;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	for (int32 i = 0; i < CachedNumToSpawn; i++)
	{
		FVector RandomLocation;
		UNavigationSystemV1::K2_GetRandomPointInNavigableRadius(World, CachedSpawnOrigin, RandomLocation, CachedRandomSpawnRadius);
		RandomLocation+=FVector(0.f, 0.f, CachedSpwanLocationZOffset);

		const FRotator& SpwanRotation = AbilitySystemComponent->GetAvatarActor()->GetActorForwardVector().ToOrientationRotator();
		AEnemyCharacter*SpawnedEnemy=World->SpawnActor<AEnemyCharacter>(LoadedClass, RandomLocation, SpwanRotation, SpawnParameters);
		if (SpawnedEnemy)
		{
			SpawnwdEnemies.Add(SpawnedEnemy);
		}
	}
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (!SpawnwdEnemies.IsEmpty())
		{
			OnSpawnFinished.Broadcast(SpawnwdEnemies);
		}
		else
		{
			DidNotSpawn.Broadcast(SpawnwdEnemies);
		}
	}
	EndTask();
}
