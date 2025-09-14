// Fill out your copyright notice in the Description page of Project Settings.


#include "Controllers/WarriorAIController.h"
#include<DebugHelper/Debug.h>

#include<Navigation/CrowdFollowingComponent.h>
#include<Perception/AIPerceptionComponent.h>
#include<Perception/AISenseConfig_Sight.h>
#include<BehaviorTree/BlackboardComponent.h>
#include<WarriorGameplayTags.h>
#include<WarriorFunctionLibrary.h>
#include<Kismet/KismetMathLibrary.h>
AWarriorAIController::AWarriorAIController(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UCrowdFollowingComponent>("PathFollowingComponent"))
{
	
	AISenseConfig_Sight = CreateDefaultSubobject<UAISenseConfig_Sight>("EnemySenseConfig_Sight");
	AISenseConfig_Sight->DetectionByAffiliation.bDetectEnemies = true;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectFriendlies = false;
	AISenseConfig_Sight->DetectionByAffiliation.bDetectNeutrals = false;
	AISenseConfig_Sight->SightRadius = 5000.f;
	AISenseConfig_Sight->LoseSightRadius = 0.f;
	AISenseConfig_Sight->PeripheralVisionAngleDegrees = 360.f;
	EnemyPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("EnemyPerceptionComponent");
	EnemyPerceptionComponent->ConfigureSense(*AISenseConfig_Sight);
	EnemyPerceptionComponent->SetDominantSense(UAISenseConfig_Sight::StaticClass());
	EnemyPerceptionComponent->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &AWarriorAIController::OnEnemyPerceptionUpdated);
	EnemyPerceptionComponent->OnTargetPerceptionForgotten.AddUniqueDynamic(this, &AWarriorAIController::OnEnemyPerceptionForgotten);
	SetGenericTeamId(FGenericTeamId(1));
}

void AWarriorAIController::BeginPlay()
{
	Super::BeginPlay();
	if (UCrowdFollowingComponent* CrowdComponent=Cast<UCrowdFollowingComponent>(GetPathFollowingComponent()))
	{
		CrowdComponent->SetCrowdSimulationState(bEnableDetourCrowdAvoidance ? ECrowdSimulationState::Enabled : ECrowdSimulationState::Disabled);
		switch (DetourCrowdAvoidanceQuality)
		{
		case 1:
			CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Low);
			break;
		case 2:
			CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Medium);
			break;
		case 3:
			CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::Good);
			break;
		case 4:
			CrowdComponent->SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);
			break;
		default:
			break;
		}
		CrowdComponent->SetAvoidanceGroup(1);
		CrowdComponent->SetGroupsToAvoid(1);
		CrowdComponent->SetCrowdCollisionQueryRange(CollisionQueryRange);
	}
}

ETeamAttitude::Type AWarriorAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const APawn*PawnToCheck=Cast<const APawn>(&Other);
	const IGenericTeamAgentInterface*OtherAgent=Cast<const IGenericTeamAgentInterface>(PawnToCheck->GetController());
	if (OtherAgent && OtherAgent->GetGenericTeamId() <GetGenericTeamId())
	{
		return ETeamAttitude::Hostile;
	}
	return ETeamAttitude::Friendly;
}

void AWarriorAIController::OnEnemyPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		if (!BlackboardComponent->GetValueAsObject(FName("TargetActor")))
		{
			if (Stimulus.WasSuccessfullySensed() && Actor)
			{
				BlackboardComponent->SetValueAsObject(FName("TargetActor"), Actor);
				UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(GetPawn(), WarriorGameplayTags::Shared_Status_TargetLock);
			}
		}
	}
}

void AWarriorAIController::OnEnemyPerceptionForgotten(AActor* Actor)
{
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetPawn(), WarriorGameplayTags::Shared_Status_TargetLock);
}
