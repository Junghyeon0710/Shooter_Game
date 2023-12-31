// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animaion/GruxAnimInstance.h"
#include <../Public/Character/Enemy.h>

void UGruxAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Enemy == nullptr)
	{
		Enemy = Cast<AEnemy>(TryGetPawnOwner());
	}

	if (Enemy)
	{
		FVector Velocity = Enemy->GetVelocity();
		Velocity.Z = 0.f;
		EnemySpeed = Velocity.Size();
	}
}

void UGruxAnimInstance::NativeInitializeAnimation()
{
	Enemy = Cast<AEnemy>(TryGetPawnOwner());
}
