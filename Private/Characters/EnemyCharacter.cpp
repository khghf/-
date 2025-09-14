// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/EnemyCharacter.h"


#include<GameFramework/CharacterMovementComponent.h>
#include<Components/Combat/EnemyCombatComponent.h>
#include<Components/UI/EnemyUIComponent.h>
#include<Components/WidgetComponent.h>
#include<Components/BoxComponent.h>
#include<Engine/AssetManager.h>
#include "DateAssets/StartUpData/DataAsset_StartUpDataBase.h"
#include<Widgets/WarriorWidgetBase.h>
#include<WarriorFunctionLibrary.h>
#include<GameModes/CombatGameModeBase.h>
#include<WarriorGameplayTags.h>
AEnemyCharacter::AEnemyCharacter()
{
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);
	GetCharacterMovement()->MaxWalkSpeed = 300.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 1000.f;
	EnemyCombatComponent = CreateDefaultSubobject<UEnemyCombatComponent>("EnemyCombatComponent");
	EnemyUIComponent = CreateDefaultSubobject<UEnemyUIComponent>("EnemyUIComponent");
	EnemyHelathWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("EnemyHelathWidgetComponent");
	EnemyHelathWidgetComponent->SetupAttachment(GetMesh());

	LeftHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("LeftHandCollisionBox");
	LeftHandCollisionBox->SetupAttachment(GetMesh());
	LeftHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
	LeftHandCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxEndOverlap);

	RightHandCollisionBox = CreateDefaultSubobject<UBoxComponent>("RightHandCollisionBox");
	RightHandCollisionBox->SetupAttachment(GetMesh());
	RightHandCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxBeginOverlap);
	RightHandCollisionBox->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::OnBodyCollisionBoxEndOverlap);
	
}

void AEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	InitEnemyStartUpData(); 
	UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(this, WarriorGameplayTags::Shared_Status_Walking);
}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (UWarriorWidgetBase* HealthWidget = Cast<UWarriorWidgetBase>(EnemyHelathWidgetComponent->GetUserWidgetObject()))
	{
		HealthWidget->InitEnemyCreatedWidget(this);
	}
}
#if WITH_EDITOR
void AEnemyCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, LeftHandCollisionBoxAttachBoneName))
	{
		LeftHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandCollisionBoxAttachBoneName);
	}
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, RightHandCollisionBoxAttachBoneName))
	{
		RightHandCollisionBox->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandCollisionBoxAttachBoneName);
	}
}
#endif
UPawnCombatComponent* AEnemyCharacter::GetPawnCombatComponent() const
{
	return EnemyCombatComponent;
}

UPawnUIComponent* AEnemyCharacter::GetPawnUIComponent() const
{
	return EnemyUIComponent;
}

UEnemyUIComponent* AEnemyCharacter::GetEnemyUIComponent() const
{
	return EnemyUIComponent;
}

void AEnemyCharacter::InitEnemyStartUpData()
{
	if (CharacterStartUpdata.IsNull())return;
	UAssetManager::GetStreamableManager().RequestAsyncLoad(
		CharacterStartUpdata.ToSoftObjectPath(),
		FStreamableDelegate::CreateLambda(
			[this]() 
			{
				if (UDataAsset_StartUpDataBase* LoadedData = CharacterStartUpdata.Get())
				{
					int32 AbilityApplyLevel = 1;
					if (ACombatGameModeBase* BaseGameMode = GetWorld()->GetAuthGameMode<ACombatGameModeBase>())
					{
						switch (BaseGameMode->GetCurrentGameDifficulty())
						{
						case EWarriorGameDifficulty::Easy:
							AbilityApplyLevel = 1;
							break;
						case EWarriorGameDifficulty::Normal:
							AbilityApplyLevel = 2;
							break;
						case EWarriorGameDifficulty::Hard:
							AbilityApplyLevel = 3;
							break;
						case EWarriorGameDifficulty::VeryHard:
							AbilityApplyLevel = 4;
							break;
						default:
							break;
						}
					}
					LoadedData->GiveToAbilitySystemComponent(WarriorAbilitySystemComponent);
				}
			}
		)
		);
}

void AEnemyCharacter::OnBodyCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APawn* HitPawn = Cast<APawn>(OtherActor))
	{
		if (UWarriorFunctionLibrary::IsTargetPawnHostile(this, HitPawn))
		{
			EnemyCombatComponent->OnHitTargetActor(HitPawn);
		}
	}
}

void AEnemyCharacter::OnBodyCollisionBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
