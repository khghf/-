// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PawnUIInterface.generated.h"
class UPawnUIComponent;
class UHeroUIComponent;
class UEnemyUIComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPawnUIInterface : public UInterface
{
	GENERATED_BODY()
};

/**
角色类需实现的接口，用于获取UIComponent,
UIComponent用于传递UI所需的数据，中介模式
 */
class PRACTICEDEMO_API IPawnUIInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UPawnUIComponent* GetPawnUIComponent()const = 0;
	virtual UHeroUIComponent* GetHeroUIComponent()const;
	virtual UEnemyUIComponent* GetEnemyUIComponent()const;
};
