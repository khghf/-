// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_RotateToFaceTarget.h"


#include<BehaviorTree/BlackboardComponent.h>
#include<AIController.h>
#include<Kismet/KismetMathLibrary.h>
UBTTask_RotateToFaceTarget::UBTTask_RotateToFaceTarget()
{
	NodeName = TEXT("Native Rotate To Face Target Actor");
	AnglePrecision = 10.f;
	RotationInterpSpeed = 5.f;
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = false;
	INIT_TASK_NODE_NOTIFY_FLAGS();
	InTargetToFaceKey.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, InTargetToFaceKey), AActor::StaticClass());
}

void UBTTask_RotateToFaceTarget::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	if (UBlackboardData* BBAsset = GetBlackboardAsset())
	{
		InTargetToFaceKey.ResolveSelectedKey(*BBAsset);
	}
}

uint16 UBTTask_RotateToFaceTarget::GetInstanceMemorySize() const
{
	return sizeof(FRotateToFaceTargetMemory);
}

FString UBTTask_RotateToFaceTarget::GetStaticDescription() const
{
	FString KeyDescription = InTargetToFaceKey.SelectedKeyName.ToString();
	return FString::Printf(TEXT("Smoothly rotates to face %s key until the angle precision %.2f is reached"), *KeyDescription,AnglePrecision);
}

EBTNodeResult::Type UBTTask_RotateToFaceTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UObject* ActorObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(InTargetToFaceKey.SelectedKeyName);
	AActor* TargetActor = Cast<AActor>(ActorObject);
	APawn* OwningPawn = OwnerComp.GetAIOwner()->GetPawn();
	FRotateToFaceTargetMemory*Memory=CastInstanceNodeMemory<FRotateToFaceTargetMemory>(NodeMemory);
	check(Memory);
	Memory->OwningPawn = OwningPawn;
	Memory->TargetActor = TargetActor;
	if (!Memory->IsValid())
	{
		return EBTNodeResult::Failed;
	}
	if (HasReachedAnglePrecision(OwningPawn, TargetActor))
	{
		Memory->Reset();
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::InProgress;
}

void UBTTask_RotateToFaceTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FRotateToFaceTargetMemory* Memory = CastInstanceNodeMemory<FRotateToFaceTargetMemory>(NodeMemory);
	if (!Memory->IsValid())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	if (HasReachedAnglePrecision(Memory->OwningPawn.Get(), Memory->TargetActor.Get()))
	{
		Memory->Reset();
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		const FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(Memory->OwningPawn.Get()->GetActorLocation(), Memory->TargetActor.Get()->GetActorLocation());
		const FRotator TargetRot = UKismetMathLibrary::RInterpTo(Memory->OwningPawn.Get()->GetActorRotation(), LookAtRot, RotationInterpSpeed, DeltaSeconds);
		Memory->OwningPawn.Get()->SetActorRotation(FRotator(0.f,TargetRot.Yaw,0.f));
	}
}

bool UBTTask_RotateToFaceTarget::HasReachedAnglePrecision(APawn* QueryPawn, AActor* TargetActor) const
{
	const FVector QueryPawnForward = UKismetMathLibrary::GetForwardVector(QueryPawn->GetActorRotation());
	const FVector QuerierToTargetNormalized = UKismetMathLibrary::Subtract_VectorVector(TargetActor->GetActorLocation(), QueryPawn->GetActorLocation()).GetSafeNormal();
	const float Angle = UKismetMathLibrary::DegAcos(FVector::DotProduct(QueryPawnForward, QuerierToTargetNormalized));
	return Angle <= AnglePrecision;
}
