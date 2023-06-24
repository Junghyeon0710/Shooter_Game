// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Weapon.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_GAME_API AWeapon : public AItem
{
	GENERATED_BODY()
public:
	AWeapon();

	virtual void Tick(float DeltaTiem) override;
protected:
	
	void StopFalling();
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling=false;
public:
	void ThrowWeapon();
};
