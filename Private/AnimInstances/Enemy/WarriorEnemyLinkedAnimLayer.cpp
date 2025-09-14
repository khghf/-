// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstances/Enemy/WarriorEnemyLinkedAnimLayer.h"

#include<AnimInstances/Enemy/WarriorEnemyAnimInstance.h>
UWarriorEnemyAnimInstance* UWarriorEnemyLinkedAnimLayer::GetEnemyAnimInstance() const
{
    return Cast<UWarriorEnemyAnimInstance>(GetOwningComponent()->GetAnimInstance());
}
