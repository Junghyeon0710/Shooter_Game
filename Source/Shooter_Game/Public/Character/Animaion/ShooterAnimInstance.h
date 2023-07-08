// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Hip UMETA(DisplayName = "Hip"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_Max UMETA(DisplayName = "DefaultMax")
};
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

	/** 자리에서 돌았나 */
	void TurnInPlace();
	
	/**달릴 때 기울리는걸 계산 */
	void Lean(float DeltaTime);
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
	
	/**캐릭터 Yaw ,가만히 있을때 업데이트 됨 ,Turn In Plac 함수*/
	float TICCharacaterYaw = 0.f;

	//캐릭터 마지막 프레임 Yaw 가만히 있을때 업데이트 됨 ,Turn In Plac 함수
	float TICCharacterYawLastFrame = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float RootYawOffset = 0.f;

	// 프레임에 로테이션 커브값
	float RotationCurve;

	// 프레임에 마지막 로테이션 커브값
	float RotationCurveValueLastFrame;

	/** 에임이 향하는 Pitch*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	float Pitch = 0.f;

	/** 리로딩중*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bReloading = false;

	/** 리로딩중*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	bool bEquipping = false;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn In Place", meta = (AllowPrivateAccess = "true"))
	EOffsetState OffsetState = EOffsetState::EOS_Hip;

	//캐릭터 지금 Yaw
	FRotator CharacterYaw = FRotator(0.f);

	//캐릭터 마지막 Yaw
	FRotator CharacterYawLastFrame = FRotator(0.f);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Lean, meta = (AllowPrivateAccess = "true"))
	float YawDelta =0.f;

	// 웅크리고 앉았나
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Crouching, meta = (AllowPrivateAccess = "true"))
	bool bCrouching = false;


	//회전 조준에 따라 반동 무게 변경
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = combat, meta = (AllowPrivateAccess = "true"))
	float RecoilWeight =1.f;

	// 제자리에서 돌고 있는지
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = combat, meta = (AllowPrivateAccess = "true"))
	bool bTurningPlace = false;
};
