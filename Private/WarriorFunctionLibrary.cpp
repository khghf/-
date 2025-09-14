// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorFunctionLibrary.h"
#include<DebugHelper/Debug.h>


#include<AbilitySystemBlueprintLibrary.h>
#include<AbilitySystem/WarriorAbilitySystemComponent.h>
#include<Interfaces/PawnCombatInterface.h>
#include<GenericTeamAgentInterface.h>
#include<Kismet/KismetMathLibrary.h>
#include<Kismet/GameplayStatics.h>
#include<WarriorGameplayTags.h>
#include<WarriorTypes/WarriorCountDownAction.h>
#include<Engine/LatentActionManager.h>
#include<WarriorGameInstance.h>
#include<SaveGame/WarriorSaveGame.h>
#include<Blueprint/WidgetLayoutLibrary.h>
#include<Components/CapsuleComponent.h>
#include<Characters/BaseCharacter.h>
UWarriorAbilitySystemComponent* UWarriorFunctionLibrary::NativeGetWarriorASCFromActor(AActor* InActor)
{
    check(InActor);
    return CastChecked<UWarriorAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InActor));
}

bool UWarriorFunctionLibrary::NativeDoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    return ASC->HasMatchingGameplayTag(TagToCheck);
}

UPawnCombatComponent* UWarriorFunctionLibrary::NativeGetPawnCombatComponentFromActor(AActor* InActor)
{
    check(InActor);
    if (IPawnCombatInterface* PawnCombatInterface = Cast<IPawnCombatInterface>(InActor))
    {
        return PawnCombatInterface->GetPawnCombatComponent();
    }
    return nullptr;
}

bool UWarriorFunctionLibrary::IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn)
{
    check(QueryPawn && TargetPawn);
    IGenericTeamAgentInterface* QueryPawnAgent = Cast<IGenericTeamAgentInterface>(QueryPawn->GetController());
    IGenericTeamAgentInterface* TargetPawnAgent = Cast<IGenericTeamAgentInterface>(TargetPawn->GetController());
    if (QueryPawnAgent && TargetPawnAgent)
    {
        return QueryPawnAgent->GetGenericTeamId() != TargetPawnAgent->GetGenericTeamId();
    }
    return false;
}

bool UWarriorFunctionLibrary::NativeHasAnyThingBetweenTwoActor(const AActor* A, const AActor* B)
{
    if (!A || !B)return false;
    FHitResult HitResult;
    if (UWorld* World = A->GetWorld())
    {
        return UKismetSystemLibrary::LineTraceSingle(
            World,
            A->GetActorLocation(),
            B->GetActorLocation(),
            ETraceTypeQuery::TraceTypeQuery1,
            false,
            TArray<AActor*>(),
            EDrawDebugTrace::ForOneFrame,
            HitResult,
            true
        );
    }
    return false;
}




void UWarriorFunctionLibrary::AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    if (ASC->HasMatchingGameplayTag(TagToAdd))return;
    ASC->AddLooseGameplayTag(TagToAdd);
}

void UWarriorFunctionLibrary::RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove)
{
    UWarriorAbilitySystemComponent* ASC = NativeGetWarriorASCFromActor(InActor);
    if (!ASC->HasMatchingGameplayTag(TagToRemove))return;
    ASC->RemoveLooseGameplayTag(TagToRemove);
}

void UWarriorFunctionLibrary::BP_DoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType& OutConfirmType)
{
    OutConfirmType= NativeDoesActorHasTag(InActor,TagToCheck)? EWarriorConfirmType::Yes: EWarriorConfirmType::No;
}

UPawnCombatComponent* UWarriorFunctionLibrary::BP_GetPawnCombatComponentFromActor(AActor* InActor, EWarriorValidType& OutValidType)
{
    UPawnCombatComponent * PawnCombatComponent =NativeGetPawnCombatComponentFromActor(InActor);
    OutValidType = PawnCombatComponent ? EWarriorValidType::Valid : EWarriorValidType::InValid;
    return PawnCombatComponent;
}

float UWarriorFunctionLibrary::BP_GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat, float InLevel) 
{
    return InScalableFloat.GetValueAtLevel(InLevel);
}

FGameplayTag UWarriorFunctionLibrary::ComputeHitReactDirectionTag(AActor* InAttacker, AActor* InVictim, float& OutAngleDifference)
{
    check(InAttacker && InVictim);
    const FVector VictimForward = InVictim->GetActorForwardVector();
    const FVector VictimToAttackerNormalized = (InAttacker->GetActorLocation() - InVictim->GetActorLocation()).GetSafeNormal();
    const float DotResult = FVector::DotProduct(VictimForward, VictimToAttackerNormalized);
    OutAngleDifference = UKismetMathLibrary::DegAcos(DotResult);
    const FVector CrossResult = FVector::CrossProduct(VictimForward, VictimToAttackerNormalized);
    if (CrossResult.Z < 0.f)
    {
        OutAngleDifference *= -1.f;
    }
    if (OutAngleDifference >= -45.f && OutAngleDifference <= 45.f)
    {
        return WarriorGameplayTags::Shared_Status_HitReact_Front;
    }
    else if (OutAngleDifference < -45.f && OutAngleDifference >= -135.f)
    {
        return WarriorGameplayTags::Shared_Status_HitReact_Left;
    }
    else if (OutAngleDifference > 45.f && OutAngleDifference <= 135.f)
    {
        return WarriorGameplayTags::Shared_Status_HitReact_Right;
    }
    else
    {
        return WarriorGameplayTags::Shared_Status_HitReact_Back;
    }
}

bool UWarriorFunctionLibrary::IsValidBlock(AActor* InAttacker, AActor* InDefender)
{
    check(InAttacker && InDefender);
    const float DotResult =FVector::DotProduct(InAttacker->GetActorForwardVector(), InDefender->GetActorForwardVector());
    return DotResult < -0.1f;
}

bool UWarriorFunctionLibrary::ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor, const FGameplayEffectSpecHandle& InSpecHandle)
{
    
    UWarriorAbilitySystemComponent* SourceASC = NativeGetWarriorASCFromActor(InInstigator);
    UWarriorAbilitySystemComponent* TargetASC = NativeGetWarriorASCFromActor(InTargetActor);
    FActiveGameplayEffectHandle ActiveGameplayEffectHandle=SourceASC->ApplyGameplayEffectSpecToTarget(*InSpecHandle.Data, TargetASC);
    return ActiveGameplayEffectHandle.WasSuccessfullyApplied();
}
void UWarriorFunctionLibrary::CountDown(const UObject* WorldContextObject, float TotalTime, float UpdateInterval,float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,UPARAM(DisplayName = "OutPut") EWarriorCountDownActionOutInput& CountDownOutput, FLatentActionInfo LatentInfo)
{
    UWorld* World = nullptr;
    if (GEngine)
    {
        World=GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
    }
    if (!World)return;
    FLatentActionManager&LatentActionManager=World->GetLatentActionManager();
    FWarriorCountDownAction*FoundCountDownAction=LatentActionManager.FindExistingAction<FWarriorCountDownAction>(LatentInfo.CallbackTarget, LatentInfo.UUID);
    if (CountDownInput == EWarriorCountDownActionInput::Start)
    {
        if (!FoundCountDownAction)
        {
            LatentActionManager.AddNewAction(
                LatentInfo.CallbackTarget,
                LatentInfo.UUID,
                new FWarriorCountDownAction(TotalTime, UpdateInterval, OutRemainingTime, CountDownOutput, LatentInfo)
            );
        }
    }
    else if (CountDownInput == EWarriorCountDownActionInput::Cancel)
    {
        if (FoundCountDownAction)
        {
            FoundCountDownAction->CancelAction();
        }
    }
    else if (CountDownInput == EWarriorCountDownActionInput::RefreshNow)
    {
        if (FoundCountDownAction)
        {
            FoundCountDownAction->RefreshNow();
        }
    }
}

UWarriorGameInstance* UWarriorFunctionLibrary::GetWarriorGameInstance(const UObject* WorldContextObject)
{
    if (GEngine)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            return World->GetGameInstance<UWarriorGameInstance>();
        }
    }
    return nullptr;
}

void UWarriorFunctionLibrary::ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode)
{
    APlayerController* PlayerController = nullptr;
    if (GEngine)
    {
        if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
        {
            PlayerController = World->GetFirstPlayerController();
        }
    }
    
    FInputModeGameOnly GameOnly;
    FInputModeUIOnly UIOnly;
    if (!PlayerController)return;
    switch (InInputMode)
    {
    case EWarriorInputMode::GameOnly:
        PlayerController->SetInputMode(GameOnly);
        PlayerController->bShowMouseCursor = false;
        break;
    case EWarriorInputMode::UIOnly:
        PlayerController->SetInputMode(UIOnly);
        PlayerController->bShowMouseCursor = true;
        break;
    default:
        break;
    }
}

bool UWarriorFunctionLibrary::SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave)
{
    USaveGame* SaveGameObject=UGameplayStatics::CreateSaveGameObject(UWarriorSaveGame::StaticClass());
    if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
    {
        WarriorSaveGameObject->SavedCurrentGameDifficulty = InDifficultyToSave;
        const bool bWasSaved=UGameplayStatics::SaveGameToSlot(WarriorSaveGameObject, WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
        return bWasSaved;
    }
    return false;
}

bool UWarriorFunctionLibrary::TryLoadSavedGameDifficulty(EWarriorGameDifficulty& OutSavedDifficultyToSave)
{
    if (UGameplayStatics::DoesSaveGameExist(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0))
    {
        USaveGame* SaveGameObject = UGameplayStatics::LoadGameFromSlot(WarriorGameplayTags::GameData_SaveGame_Slot_1.GetTag().ToString(), 0);
        if (UWarriorSaveGame* WarriorSaveGameObject = Cast<UWarriorSaveGame>(SaveGameObject))
        {
            OutSavedDifficultyToSave = WarriorSaveGameObject->SavedCurrentGameDifficulty;
            return true;
        }
    }
    return false;
}

void UWarriorFunctionLibrary::BP_HasAnyThingBetweenTwoPoint(const UObject* WorldContextObject, const FVector& A, const FVector& B, EWarriorConfirmType& OutConfirmType)
{

    FHitResult HitResult;
    const bool bHas=UKismetSystemLibrary::LineTraceSingle(
        WorldContextObject,
        A,
        B,
        ETraceTypeQuery::TraceTypeQuery1,
        false,
        TArray<AActor*>(),
        EDrawDebugTrace::ForOneFrame,
        HitResult,
        true
    );
    if (bHas)
    {
        OutConfirmType = EWarriorConfirmType::Yes;
    }
    else
    {
        OutConfirmType = EWarriorConfirmType::No;
    }
}

bool UWarriorFunctionLibrary::ActorIsInScreen(AActor* InActor, APlayerController* PlayerController)
{
    if (!InActor||!PlayerController)return false;

    FVector2D ScreenSize = UWidgetLayoutLibrary::GetViewportSize(InActor);
    FVector2D ScreenPosition;
    UWidgetLayoutLibrary::ProjectWorldLocationToWidgetPosition(PlayerController, InActor->GetActorLocation(), ScreenPosition, true);
    if (
        (0 < ScreenPosition.X && ScreenPosition.X< ScreenSize.X)
        ||
        (0 < ScreenPosition.Y && ScreenPosition.Y < ScreenSize.Y)
        )
    {
        return true;
    }
    return false;
}

void UWarriorFunctionLibrary::SetGlobalTimeDilationExceptTarget(const UObject* WorldContextObject, AActor* ExceptActor, float TimeDilation)
{
    if (!ExceptActor)return;
    UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, TimeDilation);
    ExceptActor->CustomTimeDilation /=TimeDilation;
}

void UWarriorFunctionLibrary::ResetGlobalAndTargetTimeDilation(const UObject* WorldContextObject, AActor* TargetActor)
{
    if (TargetActor)
    {
        TargetActor->CustomTimeDilation = 1.f;
    }
    UGameplayStatics::SetGlobalTimeDilation(WorldContextObject, 1.f);
}

TArray<AActor*> UWarriorFunctionLibrary::GetActorsWithTargetTag(const UObject* WorldContextObject, FGameplayTag TagToCheck, FVector TraceBoxStart, FVector TraceBoxEnd, FVector TraceBoxSize, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, bool bDrawDebug)
{
    TArray<FHitResult> HitResults;
    TArray<AActor*>FindActors;
    UKismetSystemLibrary::BoxTraceMultiForObjects(
        WorldContextObject,
        TraceBoxStart,
        TraceBoxEnd,
        TraceBoxSize / 2.f,
        FRotator(),
        ObjectTypes,
        false,
        TArray<AActor*>(),
        bDrawDebug ? EDrawDebugTrace::Persistent : EDrawDebugTrace::None,
        HitResults,
        true
    );
    if (HitResults.IsEmpty()) return FindActors;
    for (const auto& Res : HitResults)
    {
        if (NativeDoesActorHasTag(Res.GetActor(), TagToCheck))
        {
            FindActors.AddUnique(Res.GetActor());
        }
    }
    return FindActors;
}

float UWarriorFunctionLibrary::GetAddedRadiusOfTwoCharacterCapsule(ACharacter* InCharacter1, ACharacter* InCharacter2)
{
    check(InCharacter1 && InCharacter2);
    const float InCharacter1CapsuleRadius = InCharacter1->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const float InCharacter2CapsuleRadius = InCharacter2->GetCapsuleComponent()->GetScaledCapsuleRadius();
    return InCharacter1CapsuleRadius + InCharacter2CapsuleRadius;
}

FVector UWarriorFunctionLibrary::GetLocationRelativeTargetCharacter(ACharacter* QueryCharacter, ACharacter* TargetCharacter, FVector Offset)
{
    check(QueryCharacter && TargetCharacter);
    const float QueryCapsuleRadius = QueryCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    const float TargetCapsuleRadius = TargetCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    FVector Ret;
    const FVector TargetForward = TargetCharacter->GetActorForwardVector();
    const FVector TargetUp = TargetCharacter->GetActorUpVector();
    const FVector TargetRight = TargetCharacter->GetActorRightVector();
    Ret =   TargetCharacter->GetActorLocation() + 
            TargetForward * (Offset.X+ QueryCapsuleRadius+ TargetCapsuleRadius) + 
            TargetUp * Offset.Z + TargetRight * Offset.Y;
    return Ret;
}

FVector UWarriorFunctionLibrary::GetCharacterCapsuleLocation(ACharacter* InCharacter)
{
    check(InCharacter);
    FVector Ret;
    FVector CapLocation = InCharacter->GetCapsuleComponent()->GetComponentLocation();
    const float HalfSize = InCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + InCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius();
    Ret = FVector(CapLocation.X, CapLocation.Y, CapLocation.Z - HalfSize);
    return Ret;
}









