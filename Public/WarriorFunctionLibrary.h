// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include<WarriorTypes/WarriorEnumTypes.h>
#include "WarriorFunctionLibrary.generated.h"

class UWarriorAbilitySystemComponent;
class UPawnCombatComponent;
struct FScalableFloat;
class UWarriorGameInstance;
UCLASS()
class PRACTICEDEMO_API UWarriorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
#pragma region Native
	static UWarriorAbilitySystemComponent* NativeGetWarriorASCFromActor(AActor* InActor);
	static bool NativeDoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck);
	static UPawnCombatComponent* NativeGetPawnCombatComponentFromActor(AActor* InActor);
	static bool NativeHasAnyThingBetweenTwoActor(const AActor*A, const AActor* B);
#pragma endregion
	
#pragma region BlueprintCallable
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static bool IsTargetPawnHostile(APawn* QueryPawn, APawn* TargetPawn);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void AddGameplayTagToActorIfNone(AActor* InActor, FGameplayTag TagToAdd);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static void RemoveGameplayTagFromActorIfFound(AActor* InActor, FGameplayTag TagToRemove);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "Does Actor Has Tag", ExpandEnumAsExecs = "OutConfirmType"))
	static void BP_DoesActorHasTag(AActor* InActor, FGameplayTag TagToCheck, EWarriorConfirmType&OutConfirmType);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (DisplayName = "Get PawnCombat Component From Actor", ExpandEnumAsExecs = "OutValidType"))
	static UPawnCombatComponent* BP_GetPawnCombatComponentFromActor(AActor* InActor,EWarriorValidType& OutValidType);
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary", meta = (CompactNodeTitle = "Get Value At Level"))
	static float BP_GetScalableFloatValueAtLevel(const FScalableFloat& InScalableFloat,float InLevel=1.f);
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static FGameplayTag ComputeHitReactDirectionTag(AActor*InAttacker,AActor*InVictim,float&OutAngleDifference);
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary")
	static bool IsValidBlock(AActor* InAttacker, AActor* InDefender);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static bool ApplyGameplayEffectSpecHandleToTargetActor(AActor* InInstigator, AActor* InTargetActor,const FGameplayEffectSpecHandle& InSpecHandle);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (Latent, WorldContext = "WorldContextObject", LatentInfo = "LatentInfo", ExpandEnumAsExecs = "CountDownInput|CountDownOutput",TotalTime="1.0", UpdateInterval="0.1"))
	static void CountDown
	(
		const UObject* WorldContextObject, float TotalTime, float UpdateInterval,
		float& OutRemainingTime, EWarriorCountDownActionInput CountDownInput,
		UPARAM(DisplayName="OutPut") EWarriorCountDownActionOutInput& CountDownOutput, FLatentActionInfo LatentInfo
	);
	UFUNCTION(BlueprintPure, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static UWarriorGameInstance* GetWarriorGameInstance(const UObject*WorldContextObject);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject"))
	static void ToggleInputMode(const UObject* WorldContextObject, EWarriorInputMode InInputMode); 
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static bool SaveCurrentGameDifficulty(EWarriorGameDifficulty InDifficultyToSave);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary")
	static bool TryLoadSavedGameDifficulty(EWarriorGameDifficulty&OutSavedDifficultyToSave);
	UFUNCTION(BlueprintCallable, Category = "Warrior|FunctionLibrary", meta = (WorldContext = "WorldContextObject", ExpandEnumAsExecs = "OutConfirmType", DisplayName = "Has Any Thing Between Two Point"))
	static void BP_HasAnyThingBetweenTwoPoint(const UObject* WorldContextObject,const FVector& A, const FVector& B, EWarriorConfirmType&OutConfirmType);
	UFUNCTION(BlueprintCallable,BlueprintPure)
	static bool ActorIsInScreen(AActor* InActor, APlayerController* PlayerController);
	//调用后需要手动恢复或者调用ResetGlobalAndTargetTimeDilation
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject",DisplayName = "BP_SetGlobalTimeDilationExceptTarget"))
	static void SetGlobalTimeDilationExceptTarget(const UObject* WorldContextObject,AActor* ExceptActor, float TimeDilation);
	//重置目标和全局的TimeDilation，主要是为了配合SetGlobalTimeDilationExceptTarget使用
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "BP_ResetGlobalAndTargetTimeDilation"))
	static void ResetGlobalAndTargetTimeDilation(const UObject* WorldContextObject, AActor* TargetActor);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "BP_GetActorsWithTargetTag"))
	static TArray<AActor*> GetActorsWithTargetTag(const UObject* WorldContextObject, FGameplayTag TagToCheck,FVector TraceBoxStart, FVector TraceBoxEnd, FVector TraceBoxSize, const TArray<TEnumAsByte<EObjectTypeQuery> >& ObjectTypes, bool bDrawDebug);
	//获取二者的胶囊体大小之和
	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (DisplayName = "BP_GetAddedRadiusOfTwoCharacterCapsule"))
	static float GetAddedRadiusOfTwoCharacterCapsule(ACharacter* InCharacter1, ACharacter* InCharacter2);
	//获取最靠目标的位置(考虑了二者的胶囊体大小，默认为目标前方，可偏移返回值)
	UFUNCTION(BlueprintCallable, BlueprintPure,meta = (DisplayName = "BP_GetLocationRelativeTargetCharacter"))
	static FVector GetLocationRelativeTargetCharacter(ACharacter* QueryCharacter, ACharacter*TargetCharacter,FVector Offset);
	UFUNCTION(BlueprintCallable,BlueprintPure ,meta = (DisplayName = "BP_GetCharacterCapsuleLocation"))
	static FVector GetCharacterCapsuleLocation(ACharacter*InCharacter);
	
#pragma endregion

	
};
