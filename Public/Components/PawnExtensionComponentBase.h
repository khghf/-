// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PawnExtensionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PRACTICEDEMO_API UPawnExtensionComponentBase : public UActorComponent
{
	GENERATED_BODY()
//*************************扩展组件基类**************************//
//只提供工具函数，不包含具体逻辑
protected:
	template<class T>
	T* GetOwningPawn()const
	{
		static_assert(TPointerIsConvertibleFromTo<T, APawn>::Value,"'T'template parameter must be derived from APawn");
		return CastChecked<T>(GetOwner());
	}
	APawn* GetOwningPawn()const
	{
		return GetOwningPawn<APawn>();
	}
	template<class T>
	T* GetOwningController()const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AController>::Value, "'T'template parameter must be derived from AController");
		return GetOwningPawn<APawn>()->GetController<T>();
	}
};
