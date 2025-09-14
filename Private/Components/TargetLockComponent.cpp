// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TargetLockComponent.h"

#include<Kismet/KismetSystemLibrary.h>
#include<Characters/HeroCharacter.h>
#include<Kismet/GameplayStatics.h>
#include<Kismet/KismetMathLibrary.h>
#include<Blueprint/WidgetLayoutLibrary.h>
#include<Blueprint/WidgetTree.h>
#include<Components/SizeBox.h>
#include "Widgets/WarriorWidgetBase.h"
#include<GameModes/CombatPlayerController.h>
#include<WarriorFunctionLibrary.h>
#include<WarriorGameplayTags.h>
#include<GameFramework/CharacterMovementComponent.h>
UTargetLockComponent::UTargetLockComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetLockComponent::BeginPlay()
{
	Super::BeginPlay();
	CachedPlayerController = Cast<APlayerController>(GetOwningPawn()->GetController());
	CachedCharacter = Cast<ACharacter>(GetOwner());
}

void UTargetLockComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!CurrentLockedActor)return;
	OnTargetLockTick(DeltaTime);
}

void UTargetLockComponent::ToggleTargetLock(bool bEnableTargetLock)
{
	if (!bEnableTargetLock)
	{
		EndLock();
	}
	else
	{
		if (bClampWalkSpeedWhenLocking)
		{
			CachedDefaultMaxWalkSpeed = CachedCharacter->GetCharacterMovement()->MaxWalkSpeed;
			CachedCharacter->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;
		}
		TryLockOnTarget();
	}
}

void UTargetLockComponent::SwitchTarget(float value)
{
	if (!CurrentLockedActor)return;
	TArray<FHitResult>BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetOwningPawn(),
		GetOwningPawn()->GetActorLocation(),
		GetOwningPawn()->GetActorLocation(),
		TraceBoxSize / 2.f,
		GetOwningPawn()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannle,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);
	const FVector PlayerLocation = GetOwningPawn()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
	float MinDistance = TNumericLimits<float>::Max();
	AActor* NewActor = nullptr;
	for (const auto& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetOwningPawn()&& HitActor!=CurrentLockedActor)
			{
				//HitActor要在屏幕内才能锁定
				if (!UWarriorFunctionLibrary::ActorIsInScreen(HitActor, CachedPlayerController))continue;
				const FVector PlayerToAvailableNormalized = (HitActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
				const FVector CrossResult = FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);
				float DisOwnerToTarget = UKismetMathLibrary::Vector_Distance(GetOwningPawn()->GetActorLocation(), HitActor->GetActorLocation());
				if (value > 0 && CrossResult.Z < 0&& DisOwnerToTarget< MinDistance)
				{
					MinDistance = DisOwnerToTarget;
					NewActor = HitActor;
				}
				if (value < 0 && CrossResult.Z > 0 && DisOwnerToTarget < MinDistance)
				{
					MinDistance = DisOwnerToTarget;
					NewActor = HitActor;
				}
			}
		}
	}
	if (NewActor)
	{
		//EndLock();
		CurrentLockedActor = NewActor;
		//TryLockOnTarget(CurrentLockedActor);
	}
	else
	{
		return;
	}
}

void UTargetLockComponent::TryLockOnTarget()
{
	CurrentLockedActor = FindActorToLock();
	TryLockOnTarget(CurrentLockedActor);
}
void UTargetLockComponent::TryLockOnTarget(AActor* LockTarget)
{
	if (LockTarget)
	{
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwner(),WarriorGameplayTags::Player_Status_Walking))
		{

			CachedGaitTag = WarriorGameplayTags::Player_Status_Walking;

		}
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwner(), WarriorGameplayTags::Player_Status_Running))
		{
			CachedGaitTag = WarriorGameplayTags::Player_Status_Running;
			TargetLockGaitTag = WarriorGameplayTags::Player_Status_Walking;
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(GetOwner(), WarriorGameplayTags::Player_Status_Walking);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetOwner(), WarriorGameplayTags::Player_Status_Running);
		}
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(GetOwner(), WarriorGameplayTags::Player_Status_TargetLock);
		DrawTargetLockWidget();
	}
	else
	{
		EndLock();
		return;
	}
}
void UTargetLockComponent::EndLock()
{
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetOwner(), WarriorGameplayTags::Player_Status_TargetLock);
	UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(GetOwner(), TargetLockGaitTag);
	UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(GetOwner(), CachedGaitTag);
	CachedCharacter->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}
AActor* UTargetLockComponent::FindActorToLock()
{
	TArray<FHitResult>BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetOwningPawn(),
		GetOwningPawn()->GetActorLocation(),
		GetOwningPawn()->GetActorLocation(),
		TraceBoxSize / 2.f,
		GetOwningPawn()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannle,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);
	float MinDistance = TNumericLimits<float>::Max();
	AActor* RetActor = nullptr;
	for (const auto& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor != GetOwningPawn())
			{
				//HitActor要在屏幕内才能锁定
				if (!UWarriorFunctionLibrary::ActorIsInScreen(HitActor, CachedPlayerController))continue;
				float DisOwnerToTarget = UKismetMathLibrary::Vector_Distance(GetOwningPawn()->GetActorLocation(), HitActor->GetActorLocation());
				if (DisOwnerToTarget < MinDistance)
				{
					MinDistance = DisOwnerToTarget;
					RetActor = HitActor;
				}
			}
		}
	}
	return RetActor;
}

void UTargetLockComponent::OnTargetLockTick(float DeltaTime)
{
	if (!CurrentLockedActor||UWarriorFunctionLibrary::NativeDoesActorHasTag(CurrentLockedActor,WarriorGameplayTags::Shared_Status_Dead))
	{
		EndLock();
		return;
	}
	UpdateCameraRotation(DeltaTime);
	SetTargetLockWidgetPosition();
}

void UTargetLockComponent::UpdateCameraRotation(float DeltaTime)
{
	const bool bShouldOverrideRotation =
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwningPawn(), WarriorGameplayTags::Player_Status_Dodging)
		&&
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetOwningPawn(), WarriorGameplayTags::Player_Status_Blocking);
	if (bShouldOverrideRotation)
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetOwningPawn()->GetActorLocation()
			, CurrentLockedActor->GetActorLocation()
		);
		LookAtRot += TargetLockCameraOffset;
		const FRotator CurrentControlRot = GetOwningPawn()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterSpeed);
		CachedPlayerController->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetOwningPawn()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}

void UTargetLockComponent::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in Blurprint"));
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(CachedPlayerController, TargetLockWidgetClass);
		check(DrawnTargetLockWidget);
		DrawnTargetLockWidget->AddToViewport();
	}

}
void UTargetLockComponent::SetTargetLockWidgetPosition()
{
	if (!CurrentLockedActor)return;
	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		CachedPlayerController,
		CurrentLockedActor->GetActorLocation(),
		ScreenPosition,
		true
	);
	if (TargetLockWidgetSize == FVector2D::ZeroVector)
	{
		DrawnTargetLockWidget->WidgetTree->ForEachWidget(
			[this](UWidget* FoundWidget)
			{
				if (USizeBox* FoundSizeBox = Cast<USizeBox>(FoundWidget))
				{
					TargetLockWidgetSize.X = FoundSizeBox->GetWidthOverride();
					TargetLockWidgetSize.Y = FoundSizeBox->GetHeightOverride();
				}
			}
		);
	}
	ScreenPosition = ScreenPosition-(TargetLockWidgetSize / 2.f)+ WidgetPositionOffset;
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition, false);
}
void UTargetLockComponent::CleanUp()
{
	
}

AActor* UTargetLockComponent::GetCurrentLockTarget()
{
	return CurrentLockedActor;
}

bool UTargetLockComponent::GetbClampWalkSpeedWhenLocking() const
{
	return bClampWalkSpeedWhenLocking;
}



