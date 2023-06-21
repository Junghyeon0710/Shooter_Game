// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

UCLASS()
class SHOOTER_GAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	/** 움직임 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MoveAction;

	/** 화면 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* LookAction;

	/** 점프 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* JumpAction;

	/** 공격 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* FireAction;

	/** 조준 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* AimingAction;

	void Move(const FInputActionValue& Value); //캐릭터 움직이기
	void Look(const FInputActionValue& Value); //캐릭터 마우스로 보기	

	void Fire(); //총쏘기


	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation);

	/** 조준버튼을 눌렀나 안눌렀나*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	//카메라 보강
	void CameraIntrerpZoom(float DeltaTime);

	// 조준했을때 화면 돌아가는거 속도 조절 
	void SetLookRates();

	//크로스헤어 퍼지는거 계산
	void CalculateCrosshairSpread(float DeltaTime);
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere ,BlueprintReadOnly,meta=(AllowPrivateAccess ="true"))
	float BaseTurnRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;

	// 조준하지 않았을 때 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float HipTurnRate = 90.f;

	// 조준하지 않았을 때 Look up 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float HipLookUpRate =90.f;
	 
	//조준했을때 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimingTurnRate = 20.f;

	//조준했을떄 위아래 회전율
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float AimingLookUpRate  =20.f;

	/** 마우스 회전율*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"),meta = (ClampMin = "0.0",Clamp = "1.0",UIMin = "0.0",UIMax="1.0"))
	float MouseHipTurnRate = 1.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", Clamp = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseHipLookUpRate = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", Clamp = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingTurnRate =0.2f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), meta = (ClampMin = "0.0", Clamp = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float MouseAimingLookUpRate =0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* MuzzleFalsh;

	/** 히트지점 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticle;

	/** 연기흔적 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BeamParticle;
	
	/** 총쏘는 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	/** 조준하고 있냐 참거짓*/
	UPROPERTY(visibleAnywhere, BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;

	/** 기본 카메라 화면 뷰 값*/
	float CameraDefaultFOV = 0;

	/** 카메라 확대했을때 줌 값*/
	float CameraZoomedFOV = 35.f;

	/** 현재 시야*/
	float CameraCurrentFOV = 0.f;

	/** 조준 확대 속도*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed =20.f;

	/**크로스헤어 퍼짐을 결정 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplire;

	/**속도 구성요소 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor;

	/**공중에 있을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor;

	/**조준 했을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrooshairAimFactor;

	/**총쏘고 있을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor;
public:
	FORCEINLINE bool GetAiming() const { return bAiming; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;
};
