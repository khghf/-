// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/HeroGameplayAbility_TargetLock.h"


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
#include<EnhancedInputSubsystems.h>
void UHeroGameplayAbility_TargetLock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	InitTargetLockMovement();
	InitTargetLockMappingContext();
	TryLockOnTarget();
}

void UHeroGameplayAbility_TargetLock::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	ResetTargetLockMovement();
	CleanUp();
	ResetTargetLockMappingContext();
	Super::EndAbility( Handle, ActorInfo, ActivationInfo,  bReplicateEndAbility,  bWasCancelled);
}

void UHeroGameplayAbility_TargetLock::OnTargetLockTick(float DeltaTime)
{
	//锁定目标
	if (!CurrentLockedActor||UWarriorFunctionLibrary::NativeDoesActorHasTag(CurrentLockedActor,WarriorGameplayTags::Shared_Status_Dead))
	{
		//主角死亡则取消能力
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Shared_Status_Dead))
		{
			CancelTargetLockAbility();
			return;
		}
		CancelTargetLockAbility();
		if (IsTryLockOnNextTargetWhenCurrentTargetWasDead())
		{
			FGameplayEventData Data;
			ActivateAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), &Data);
		}
		return;
	}
	SetTargetLockWidgetPosition();
	const bool bShouldOverrideRotation =
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Dodging)
		&&
		!UWarriorFunctionLibrary::NativeDoesActorHasTag(GetHeroCharacterFromActorInfo(), WarriorGameplayTags::Player_Status_Blocking);
	if (bShouldOverrideRotation)
	{
		FRotator LookAtRot=UKismetMathLibrary::FindLookAtRotation(GetHeroCharacterFromActorInfo()->GetActorLocation()
			, CurrentLockedActor->GetActorLocation()
		);
		LookAtRot += TargetLockCameraOffset;
		const FRotator CurrentControlRot = GetCombatPlayerControllerFromActorInfo()->GetControlRotation();
		const FRotator TargetRot = FMath::RInterpTo(CurrentControlRot, LookAtRot, DeltaTime, TargetLockRotationInterSpeed);
		GetCombatPlayerControllerFromActorInfo()->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
		GetHeroCharacterFromActorInfo()->SetActorRotation(FRotator(0.f, TargetRot.Yaw, 0.f));
	}
}


void UHeroGameplayAbility_TargetLock::SwitchTarget(const FGameplayTag& InSwitchDirectionTag)
{
	GetAvailableActorsToLock();
	TArray<AActor*>ActorsOnLeft;
	TArray<AActor*>ActorsOnRight;
	AActor* NewTargetToLock = nullptr;
	GetAvailableActorsAroundTarget(ActorsOnLeft, ActorsOnRight);
	if (InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Left)
	{
		NewTargetToLock=GetNearestTargetFromAvailableActors(ActorsOnLeft);
	}
	else if(InSwitchDirectionTag == WarriorGameplayTags::Player_Event_SwitchTarget_Right)
	{
		NewTargetToLock = GetNearestTargetFromAvailableActors(ActorsOnRight);
	}
	if (NewTargetToLock)
	{
		CurrentLockedActor = NewTargetToLock;
	}
}

void UHeroGameplayAbility_TargetLock::TryLockOnTarget()
{
	GetAvailableActorsToLock();
	if (AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility(); 
		return;
	}
	CurrentLockedActor=GetNearestTargetFromAvailableActors(AvailableActorsToLock);
	if (CurrentLockedActor)
	{
		DrawTargetLockWidget();
		SetTargetLockWidgetPosition();
	}
	else
	{
		CancelTargetLockAbility();
	}
}

bool UHeroGameplayAbility_TargetLock::IsTryLockOnNextTargetWhenCurrentTargetWasDead()
{
	if (bAutoLockOnNextTargetWhenCurrentTargetDead)
	{
		//自动锁敌逻辑，暂且搁置
		return true;
	}
	return false;
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsToLock()
{
	AvailableActorsToLock.Empty();
	TArray<FHitResult>BoxTraceHits;
	UKismetSystemLibrary::BoxTraceMultiForObjects(
		GetHeroCharacterFromActorInfo(),
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		GetHeroCharacterFromActorInfo()->GetActorLocation() + GetHeroCharacterFromActorInfo()->GetActorForwardVector() * BoxTraceDistance,
		TraceBoxSize / 2.f,
		GetHeroCharacterFromActorInfo()->GetActorForwardVector().ToOrientationRotator(),
		BoxTraceChannle,
		false,
		TArray<AActor*>(),
		bShowPersistentDebugShape ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
		BoxTraceHits,
		true
	);
	for (const auto& TraceHit : BoxTraceHits)
	{
		if (AActor* HitActor = TraceHit.GetActor())
		{
			if (HitActor!= GetHeroCharacterFromActorInfo())
			{
				//HitActor要在屏幕内才能锁定
				if (!UWarriorFunctionLibrary::ActorIsInScreen(HitActor, GetCombatPlayerControllerFromActorInfo()))continue;
				AvailableActorsToLock.AddUnique(HitActor);
			}
		}
	}
}

AActor* UHeroGameplayAbility_TargetLock::GetNearestTargetFromAvailableActors(const TArray<AActor*>& InAvailableActors)
{
	float CloseettDistance = 0.f;
	return UGameplayStatics::FindNearestActor(
		GetHeroCharacterFromActorInfo()->GetActorLocation(),
		InAvailableActors,
		CloseettDistance
		);
}

void UHeroGameplayAbility_TargetLock::GetAvailableActorsAroundTarget(TArray<AActor*>& OutActorsOnLeft, TArray<AActor*>& OutActorsOnRight)
{
	if (!CurrentLockedActor || AvailableActorsToLock.IsEmpty())
	{
		CancelTargetLockAbility();
		return;
	}
	const FVector PlayerLocation = GetHeroCharacterFromActorInfo()->GetActorLocation();
	const FVector PlayerToCurrentNormalized = (CurrentLockedActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
	for (const auto& AvailableActor : AvailableActorsToLock)
	{
		if (!AvailableActor || AvailableActor == CurrentLockedActor)continue;
		const FVector PlayerToAvailableNormalized= (AvailableActor->GetActorLocation() - PlayerLocation).GetSafeNormal();
		const FVector CrossResult=FVector::CrossProduct(PlayerToCurrentNormalized, PlayerToAvailableNormalized);
		if (CrossResult.Z < 0)
		{
			OutActorsOnLeft.AddUnique(AvailableActor);
		}
		else
		{
			OutActorsOnRight.AddUnique(AvailableActor);
		}
	}
}

void UHeroGameplayAbility_TargetLock::DrawTargetLockWidget()
{
	if (!DrawnTargetLockWidget)
	{
		checkf(TargetLockWidgetClass, TEXT("Forgot to assign a valid widget class in Blurprint"));
		DrawnTargetLockWidget = CreateWidget<UWarriorWidgetBase>(GetCombatPlayerControllerFromActorInfo(), TargetLockWidgetClass);
		check(DrawnTargetLockWidget);
		DrawnTargetLockWidget->AddToViewport();
	}
	
}

void UHeroGameplayAbility_TargetLock::SetTargetLockWidgetPosition()
{
	if (!DrawnTargetLockWidget && !CurrentLockedActor)
	{
		CancelTargetLockAbility();
		return;
	}
	FVector2D ScreenPosition;
	UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(
		GetCombatPlayerControllerFromActorInfo(),
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
	ScreenPosition -= (TargetLockWidgetSize / 2.f);
	DrawnTargetLockWidget->SetPositionInViewport(ScreenPosition,false);
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMovement()
{
	/*CachedDefaultMaxWalkSpeed = GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed;
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = TargetLockMaxWalkSpeed;*/
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->bOrientRotationToMovement = false;
}

void UHeroGameplayAbility_TargetLock::InitTargetLockMappingContext()
{
	if (ULocalPlayer* LocalPlay = GetCombatPlayerControllerFromActorInfo()->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlay);
		check(SubSystem);
		SubSystem->AddMappingContext(TargetLockMappingContext, 3);
	}
}

void UHeroGameplayAbility_TargetLock::CancelTargetLockAbility()
{
	CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
}
void UHeroGameplayAbility_TargetLock::CleanUp()
{
	AvailableActorsToLock.Empty();
	CurrentLockedActor = nullptr;
	if (DrawnTargetLockWidget)
	{
		DrawnTargetLockWidget->RemoveFromParent();
	}
	DrawnTargetLockWidget = nullptr;
	TargetLockWidgetSize = FVector2D::ZeroVector;
	CachedDefaultMaxWalkSpeed = 0.f;
}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMovement()
{
	/*if (CachedDefaultMaxWalkSpeed>0.f)
	{
		GetHeroCharacterFromActorInfo()->GetCharacterMovement()->MaxWalkSpeed = CachedDefaultMaxWalkSpeed;
	}*/
	GetHeroCharacterFromActorInfo()->GetCharacterMovement()->bOrientRotationToMovement = true;

}

void UHeroGameplayAbility_TargetLock::ResetTargetLockMappingContext()
{
	if (!GetCombatPlayerControllerFromActorInfo())return;
	if (ULocalPlayer* LocalPlay = GetCombatPlayerControllerFromActorInfo()->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem* SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlay);
		check(SubSystem);
		SubSystem->RemoveMappingContext(TargetLockMappingContext);
	}
}
