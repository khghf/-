// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameModes/CombatGameModeBase.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include "WarriorSurivalGameModeBase.generated.h"
class AEnemyCharacter;




UENUM(BlueprintType)
enum class EWarriorSurvialGameModeState :uint8
{
	WaitSpawnNewWave,
	SpawningNewWave,
	InProgress,
	WaveCompleted,
	AllWavesDone,
	PlayerDied,
};
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerInfo
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AEnemyCharacter>SoftEnemyClassToSpawn;
	//一次性生成敌人的数量限制
	UPROPERTY(EditAnywhere)
	int32 MinPreSpawnCount = 1;
	//一次性生成敌人的数量限制
	UPROPERTY(EditAnywhere)
	int32 MaxPreSpawnCount = 3;
};
USTRUCT(BlueprintType)
struct FWarriorEnemyWaveSpawnerTableRow :public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere)
	TArray<FWarriorEnemyWaveSpawnerInfo>EnemyWaveSpawnerDefinitions;
	UPROPERTY(EditAnywhere)
	int32 TotalEnemyToSpawnThisWave = 1;
};
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSurvialGameModeStateChangedDelegate, EWarriorSurvialGameModeState, CurrentSurvialGameModeState);
/**
 * 
 */
UCLASS()
class PRACTICEDEMO_API AWarriorSurivalGameModeBase : public ACombatGameModeBase
{
	GENERATED_BODY()



protected:
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)override;

	virtual void BeginPlay()override;
	virtual void Tick(float DeltaTime)override;
private:
	void SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState);
	bool HasFinishedAllWaves()const;
	//加载下一波敌人所需的资源
	void PreLoadNextWaveEnemies();
	int32 TrySpawnWaveEnemies();
	bool ShouldKeepSpawnEnemies()const;
	FWarriorEnemyWaveSpawnerTableRow* GetCurrentWaveSpawnerTableRow()const;
	UFUNCTION()
	void OnEnemyDestroyed(AActor* DestroyedActor);
	UPROPERTY()
	EWarriorSurvialGameModeState CurrentSurvialGameModeState;
	UPROPERTY(BlueprintAssignable,BlueprintCallable)
	FOnSurvialGameModeStateChangedDelegate OnSurvialGameModeStateChanged;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess="true"))
	UDataTable* EnemyWaveSpawnerDataTable;
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 TotalWavesToSpawn;
	//当前波次
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	int32 CurrentWaveCount = 1;
	UPROPERTY()
	int32 CurrentSpawnedEnemiesCounter = 0;
	UPROPERTY()
	int32 TotalSpawnedEnemiesThisWaveCounter = 0;
	UPROPERTY()
	TArray<AActor*>TargetPonitsArray;
	UPROPERTY()
	float TimePassedSinceStart=0.f;
	//开始新波次前的等待时间
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnNewWaveWaitTime=5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float SpawnEnemysDelayTime=2.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WaveDefinition", meta = (AllowPrivateAccess = "true"))
	float WaveCompletedWaitTime=5.f;
	//存储预加载的下一波要生成的敌人的类型
	UPROPERTY()
	TMap<TSoftClassPtr<AEnemyCharacter>, UClass*>PreLoadedEnemyClassMap;
public:
	UFUNCTION(BlueprintCallable)
	void RegisterSpawnedEnemies(const TArray<AEnemyCharacter*>& InEnemiesToRegiater);
};
