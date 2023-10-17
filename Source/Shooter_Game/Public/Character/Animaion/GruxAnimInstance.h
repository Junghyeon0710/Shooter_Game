// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GruxAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_GAME_API UGruxAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;

private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Movemont,meta=(AllowprivateAccess="true"))
	float EnemySpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movemont, meta = (AllowprivateAccess = "true"))
	class AEnemy* Enemy;
};
