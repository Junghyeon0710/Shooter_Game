// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_GAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
		void UpdateAnimationProperties(float DeltaTime);

	virtual void NativeInitializeAnimation() override;
protected:

	/** */
	void TurnInPlace();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
		class AMyCharacter* ShooterCharacter;

	/** 캐릭터 스피드*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float Speed = 0;

	/** 캐릭터 공중에 있는지*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsInAir = false;

	/** 캐릭터가 움직이는지*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bIsAccelerating = false;//가속도

	//스트라이핑에 사용되는 오프셋 요
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float MoventOffsetYaw = 0;

	//멈추기전의 움직임
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float LastMoventOffsetYaw = 0.f;

	//조준중인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;
	
	/**캐릭터 Yaw */
	float CharacaterYaw = 0.f;

	//캐릭터 마지막 프리엠 Yaw
	float CharacterYawLastFrame = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset = 0.f;

	// 프레임에 로테이션 커브값
	float RotationCurve;

	// 프레임에 마지막 로테이션 커브값
	float RotationCurveValueLastFrame;

};
