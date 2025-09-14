// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/WarriorSurivalGameModeBase.h"

#include<Engine/AssetManager.h>
#include<Engine/TargetPoint.h>
#include<Characters/EnemyCharacter.h>
#include<Kismet/GameplayStatics.h>
#include<NavigationSystem.h>
#include<WarriorFunctionLibrary.h>
void AWarriorSurivalGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	EWarriorGameDifficulty SavedDifficulty;
	if (UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(SavedDifficulty))
	{
		CurrentGameDifficulty = SavedDifficulty;
	}
}
void AWarriorSurivalGameModeBase::BeginPlay()
{
	Super::BeginPlay();
	checkf(EnemyWaveSpawnerDataTable, TEXT("Forgot to assign a valid data table in *s game mode blueprint"),*GetActorNameOrLabel());
	SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);
	TotalWavesToSpawn=EnemyWaveSpawnerDataTable->GetRowNames().Num();
	PreLoadNextWaveEnemies();
}

void AWarriorSurivalGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//倒计时
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaitSpawnNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnNewWaveWaitTime)
		{
			TimePassedSinceStart = 0.f;
			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::SpawningNewWave);
		}
	}
	//完成倒计时后延迟SpawnEnemysDelayTime的时间再生成敌人
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::SpawningNewWave)
	{
		TimePassedSinceStart += DeltaTime;
		if (TimePassedSinceStart >= SpawnEnemysDelayTime)
		{
			CurrentSpawnedEnemiesCounter+=TrySpawnWaveEnemies();
			TimePassedSinceStart = 0.f;
			SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::InProgress);

		}
	}
	
	if (CurrentSurvialGameModeState == EWarriorSurvialGameModeState::WaveCompleted)
	{
		TimePassedSinceStart += DeltaTime;
		//完成当前波次后等待WaveCompletedWaitTime时间后再进行接下来的逻辑
		if (TimePassedSinceStart >= WaveCompletedWaitTime)
		{
			TimePassedSinceStart = 0.f;
			CurrentWaveCount++;
			if (HasFinishedAllWaves())
			{
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::AllWavesDone);
			}
			else
			{
				SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaitSpawnNewWave);
				PreLoadNextWaveEnemies();
			}
		}
	}
}

void AWarriorSurivalGameModeBase::SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState InState)
{
	CurrentSurvialGameModeState = InState;
	OnSurvialGameModeStateChanged.Broadcast(CurrentSurvialGameModeState);
}

bool AWarriorSurivalGameModeBase::HasFinishedAllWaves() const
{

	return CurrentWaveCount>TotalWavesToSpawn;
}

void AWarriorSurivalGameModeBase::PreLoadNextWaveEnemies()
{
	if (HasFinishedAllWaves())return;
	PreLoadedEnemyClassMap.Empty();
	for (const FWarriorEnemyWaveSpawnerInfo& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())continue;
		UAssetManager::GetStreamableManager().RequestAsyncLoad(
			SpawnerInfo.SoftEnemyClassToSpawn.ToSoftObjectPath(),
			FStreamableDelegate::CreateLambda(
				[SpawnerInfo,this]()
				{
					if (UClass* LoadedClass = SpawnerInfo.SoftEnemyClassToSpawn.Get())
					{
						PreLoadedEnemyClassMap.Emplace(SpawnerInfo.SoftEnemyClassToSpawn, LoadedClass);
					}
				}
			)
		);
	}
}

int32 AWarriorSurivalGameModeBase::TrySpawnWaveEnemies()
{
	//设置生成位置和旋转
	if (TargetPonitsArray.IsEmpty())
	{
		UGameplayStatics::GetAllActorsOfClass(this, ATargetPoint::StaticClass(), TargetPonitsArray);
	}
	checkf(!TargetPonitsArray.IsEmpty(), TEXT("No valid target point found in level:%s for spawning enemies"),*GetWorld()->GetName());
	uint32 EnemiesSpawnedThisTime = 0;

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	for (const auto& SpawnerInfo : GetCurrentWaveSpawnerTableRow()->EnemyWaveSpawnerDefinitions)
	{
		if (SpawnerInfo.SoftEnemyClassToSpawn.IsNull())continue;
		const int32 NumToSpawn = FMath::RandRange(SpawnerInfo.MinPreSpawnCount, SpawnerInfo.MaxPreSpawnCount);
		UClass*LoadedEnemyClass=PreLoadedEnemyClassMap.FindChecked(SpawnerInfo.SoftEnemyClassToSpawn);
		for (int32 i = 0; i < NumToSpawn; i++)
		{
			const int32 RandomTargetPointIndex = FMath::RandRange(0, TargetPonitsArray.Num() - 1);
			const FVector SpawnOrigin=TargetPonitsArray[RandomTargetPointIndex]->GetActorLocation();
			const FRotator SpawnRotation=TargetPonitsArray[RandomTargetPointIndex]->GetActorForwardVector().ToOrientationRotator();
			FVector RandomLocation;
			UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
				this, SpawnOrigin, 
				RandomLocation, 400.f
			);
			RandomLocation += FVector(0.f, 0.f, 150.f);
			AEnemyCharacter*SpawnedEnemy=GetWorld()->SpawnActor<AEnemyCharacter>(LoadedEnemyClass, RandomLocation, SpawnRotation, SpawnParameters);
			if (SpawnedEnemy)
			{
				SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
				EnemiesSpawnedThisTime++;
				TotalSpawnedEnemiesThisWaveCounter++;
			}
			if (!ShouldKeepSpawnEnemies())
			{
				return EnemiesSpawnedThisTime;
			}

		}
	}
	return EnemiesSpawnedThisTime;
}

bool AWarriorSurivalGameModeBase::ShouldKeepSpawnEnemies() const
{
	return TotalSpawnedEnemiesThisWaveCounter<GetCurrentWaveSpawnerTableRow()->TotalEnemyToSpawnThisWave;
}

FWarriorEnemyWaveSpawnerTableRow* AWarriorSurivalGameModeBase::GetCurrentWaveSpawnerTableRow() const
{
	const FName RowName = FName(TEXT("Wave") + FString::FromInt(CurrentWaveCount));
	FWarriorEnemyWaveSpawnerTableRow*FoundRow=EnemyWaveSpawnerDataTable->FindRow<FWarriorEnemyWaveSpawnerTableRow>(RowName, FString());
	checkf(FoundRow, TEXT("Could not find a valid row under the name %s in the data table ") ,*RowName.ToString());
	return FoundRow;
}

void AWarriorSurivalGameModeBase::OnEnemyDestroyed(AActor* DestroyedActor)
{
	CurrentSpawnedEnemiesCounter--;
	if (ShouldKeepSpawnEnemies())
	{
		CurrentSpawnedEnemiesCounter += TrySpawnWaveEnemies();
	}
	else if (CurrentSpawnedEnemiesCounter == 0)
	{
		TotalSpawnedEnemiesThisWaveCounter = 0;
		CurrentSpawnedEnemiesCounter = 0;
		SetCurrentSurvialGameModeState(EWarriorSurvialGameModeState::WaveCompleted);
	}

}

void AWarriorSurivalGameModeBase::RegisterSpawnedEnemies(const TArray<AEnemyCharacter*>& InEnemiesToRegiater)
{
	for (const auto& SpawnedEnemy : InEnemiesToRegiater)
	{
		if (SpawnedEnemy)
		{
			CurrentSpawnedEnemiesCounter++;
			SpawnedEnemy->OnDestroyed.AddUniqueDynamic(this, &ThisClass::OnEnemyDestroyed);
		}
	}
}
