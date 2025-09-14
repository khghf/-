 // Fill out your copyright notice in the Description page of Project Settings.
#include "Characters/HeroCharacter.h"
#include<DebugHelper/Debug.h>


#include<GameFramework/SpringArmComponent.h>
#include<GameFramework/CharacterMovementComponent.h>
#include<Camera/CameraComponent.h>
#include<EnhancedInputSubsystems.h>
#include "DateAssets/Input/DataAsset_InputConfig.h"
#include "Components/Input/WarriorEnhancedInputComponent.h"
#include<WarriorGameplayTags.h>
#include<Kismet/KismetMathLibrary.h>
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<AbilitySystem/WarriorAttributeSet.h>
#include "DateAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include<Components/Combat/HeroCombatComponent.h>
#include<Components/UI/HeroUIComponent.h>
#include<AbilitySystemBlueprintLibrary.h>
#include<GameModes/CombatGameModeBase.h>
#include<WarriorFunctionLibrary.h>
#include "Components/Input/WarriorPreInputBufferComponent.h"
#include<Components/TargetLockComponent.h>
AHeroCharacter::AHeroCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 200.f;
	CameraBoom->SocketOffset = FVector(0.f, 55.f, 65.f);
	CameraBoom->bUsePawnControlRotation = true;

	FllowCamera = CreateDefaultSubobject<UCameraComponent>("FllowCamera");
	FllowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FllowCamera->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 500.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = DefaultCrouchWalkSpeed;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	HeroCombatComponent = CreateDefaultSubobject<UHeroCombatComponent>("HeroCombatComponent");
	HeroUIComponent = CreateDefaultSubobject<UHeroUIComponent>("HeroUIComponent");
	PreInputBufferCom = CreateDefaultSubobject<UWarriorPreInputBufferComponent>("PreInputBufferCom");
	PreInputBufferCom->OnBufferConsumed.BindUObject(this, &ThisClass::Input_AbilityInputPressed);
	TargetLocakCom = CreateDefaultSubobject<UTargetLockComponent>("TargetLocakCom");

}

void AHeroCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset, TEXT("Forgot assign a valid data asset as input config"));
	if (ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer())
	{
		UEnhancedInputLocalPlayerSubsystem*SubSystem=ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
		UWarriorEnhancedInputComponent* WarriorInputComponent = CastChecked<UWarriorEnhancedInputComponent>(PlayerInputComponent);
		SubSystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext, 0);
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this,&ThisClass::IA_Move_Func);
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Look, ETriggerEvent::Triggered, this, &ThisClass::IA_Look_Func);
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_Switchable_BetweenWalkAndRun, ETriggerEvent::Started, this, &ThisClass::IA_SwitchBetweenWalkAndRun_Func);
		
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Triggered, this, &ThisClass::IA_SwitchTargetTriggered_Func);
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_SwitchTarget, ETriggerEvent::Completed, this, &ThisClass::IA_SwitchTargetCompleted_Func);
		
		WarriorInputComponent->BindNativeInputAction(InputConfigDataAsset, WarriorGameplayTags::InputTag_PickUp_Stones, ETriggerEvent::Started, this, &ThisClass::IA_PickUpStonesStarted_Func);
		
		WarriorInputComponent->BindAbilityInputAction(InputConfigDataAsset,this,&ThisClass::Input_AbilityInputPressed,&ThisClass::Input_AbilityInputReleased);
	}
}

void AHeroCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!CharacterStartUpdata.IsNull())
	{
		if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpdata.LoadSynchronous())
		{
			int32 AbilityApplyLevel = 1;
			if (ACombatGameModeBase* BaseGameMode = GetWorld()->GetAuthGameMode<ACombatGameModeBase>())
			{
				switch (BaseGameMode->GetCurrentGameDifficulty())
				{
				case EWarriorGameDifficulty::Easy:
					AbilityApplyLevel = 4;
					break;
				case EWarriorGameDifficulty::Normal:
					AbilityApplyLevel = 3;
					break;
				case EWarriorGameDifficulty::Hard:
					AbilityApplyLevel = 2;
					break;
				case EWarriorGameDifficulty::VeryHard:
					AbilityApplyLevel = 1;
					break;
				default:
					break;
				}
			}
			LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent, AbilityApplyLevel);
		}
	}
	UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this, WarriorGameplayTags::Player_Status_Walking);
}

void AHeroCharacter::IA_Move_Func(const FInputActionValue& Value)
{
	const FVector2D& MovementValue = Value.Get<FVector2D>();
	const FRotator& MovementRotation = FRotator(0.f, Controller->GetControlRotation().Yaw, 0.f);
	if (MovementValue.X != 0.f)
	{
		const FVector& Forward = UKismetMathLibrary::GetForwardVector(MovementRotation);
		AddMovementInput(Forward, MovementValue.X);
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_RecoveryCancelAllowed))
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				this,
				WarriorGameplayTags::Player_Event_CancelRecovery,
				FGameplayEventData()
			);
		}
	}
	if (MovementValue.Y != 0.f)
	{
		const FVector& Right = UKismetMathLibrary::GetRightVector(MovementRotation);
		AddMovementInput(Right, MovementValue.Y);
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_RecoveryCancelAllowed))
		{
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
				this,
				WarriorGameplayTags::Player_Event_CancelRecovery,
				FGameplayEventData()
			);
		}
	}
	
}

void AHeroCharacter::IA_Look_Func(const FInputActionValue& Value)
{
	const FVector2D&LookValue = Value.Get<FVector2D>();
	AddControllerYawInput(LookValue.X);
	AddControllerPitchInput(-LookValue.Y);
}

void AHeroCharacter::IA_SwitchBetweenWalkAndRun_Func(const FInputActionValue& Value)
{
	if (TargetLocakCom->GetbClampWalkSpeedWhenLocking())
	{
		if (UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_TargetLock))
		{
			UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this, WarriorGameplayTags::Player_Status_Walking);
			UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(this, WarriorGameplayTags::Player_Status_Running);
			return;
		}
	}
	if (UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_Walking))
	{
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this, WarriorGameplayTags::Player_Status_Running);
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this)->RemoveLooseGameplayTag(WarriorGameplayTags::Player_Status_Walking);
		GetCharacterMovement()->MaxWalkSpeed = DefaultRunSpeed;
		return;
	}
	if (UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_Running))
	{
		UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this, WarriorGameplayTags::Player_Status_Walking);
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(this)->RemoveLooseGameplayTag(WarriorGameplayTags::Player_Status_Running);
		GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		return;
	}
}

void AHeroCharacter::IA_SwitchTargetTriggered_Func(const FInputActionValue& Value)
{
	SwitchDirection = Value.Get<FVector2D>();
}

void AHeroCharacter::IA_SwitchTargetCompleted_Func(const FInputActionValue& Value)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		SwitchDirection.X > 0 ? WarriorGameplayTags::Player_Event_SwitchTarget_Right : WarriorGameplayTags::Player_Event_SwitchTarget_Left,
		Data
	);
}

void AHeroCharacter::IA_PickUpStonesStarted_Func(const FInputActionValue& Value)
{
	FGameplayEventData Data;
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
		this,
		WarriorGameplayTags::Player_Event_ConsumeStones,
		Data
	);
}

void AHeroCharacter::Input_AbilityInputPressed(FGameplayTag InputTag)
{
	//用于结束角色攻击后摇，并立即进行下一连击
	if (
		UWarriorFunctionLibrary::NativeDoesActorHasTag(this, WarriorGameplayTags::Player_Status_RecoveryCancelAllowed)
		)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(
			this,
			WarriorGameplayTags::Player_Event_CancelRecovery,
			FGameplayEventData()
		);
	}
	PreInputBufferCom->StoreInput(InputTag);
	WarriorAbilitySystemComponent->OnAbilityInputPressed(InputTag);
}

void AHeroCharacter::Input_AbilityInputReleased(FGameplayTag InputTag)
{
	WarriorAbilitySystemComponent->OnAbilityInputReleased(InputTag);
}

UPawnCombatComponent* AHeroCharacter::GetPawnCombatComponent() const
{
	return HeroCombatComponent;
}

UHeroUIComponent* AHeroCharacter::GetHeroUIComponent() const
{
	return HeroUIComponent;
}

UPawnUIComponent* AHeroCharacter::GetPawnUIComponent() const
{
	return HeroUIComponent;
}




