// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "../AmmoType.h"
#include "MyCharacter.generated.h"


UENUM(BlueprintType)
enum class ECombatState : uint8
{
	ECS_Unoccupied UMETA(Display = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(Display = "FireTimerInProgress"),
	ECS_Reloading UMETA(Display = "Reloading"),
	ECS_Equipping UMETA(Display = "Equipping"),
	ECS_Stuneed UMETA(Display = "Stuneed"),

	ECS_Max UMETA(Display = "DefaultMax")
};
USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 ItemCount;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FHighlightIconDelegate, int32, SlotIndex, bool, bStartAniamion);

UCLASS()
class SHOOTER_GAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

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

	/** 무기 장착 해제 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* SelectAction;

	/** 무기 재장전 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* ReloadAction;

	/** F키 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* CrouchAction;

	/** F키 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* FKeyhAction;

	/** 1Key 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* Key1Action;

	/** 2Key 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* Key2Action;

	/** 3Key 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* Key3Action;

	/** 4Key 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* Key4Action;

	/** 5Key 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* Key5Action;


	void Move(const FInputActionValue& Value); //캐릭터 움직이기
	void Look(const FInputActionValue& Value); //캐릭터 마우스로 보기	

	void Fire(); //총쏘기

	virtual void Jump() override;

	bool GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult);

	/** 조준버튼을 눌렀나 안눌렀나*/
	void AimingButtonPressed();
	void AimingButtonReleased();

	void SelectButtonPressed();

	//R키 눌렀나
	void ReloadButtonPressed();
	//무기 재장전
	void ReloadWeapon();
	void ReloadMontagePlay();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	//시프트키 누르면 웅크림

	void CrouchingPressed();

	// 인벤토리 키
	void FKeyPressed();
	void Key1Pressed();
	void Key2Pressed();
	void Key3Pressed();
	void Key4Pressed();
	void Key5Pressed();

	void ExchangeInventoryIrems(int32 CurrentItemIndex, int32 NewItemIndex);

	void PlayEquipMontage();

	//카메라 보강
	void CameraIntrerpZoom(float DeltaTime);

	// 조준했을때 화면 돌아가는거 속도 조절 
	void SetLookRates();

	//크로스헤어 퍼지는거 계산
	void CalculateCrosshairSpread(float DeltaTime);

	//총솔떄 크로스헤어 퍼짐
	void StartCrosshairBulletFire();

	UFUNCTION()
	void FinishCrosshairBulletFire();

	void FireButtonPressed();
	void FireButtonReleased();

	void StartFireTimer();

	UFUNCTION()
	void AutoFireReset();

	/**라인트레이스가 향한 십자선 아래 무엇이 있나*/
	bool TraceUnderCrosshairs(FHitResult& OutHitResult,FVector& OutHitLocation);

	/** 겹친 아이템이 0개보다 많으면*/
	void TraceForItems();

	/**무기 스폰 */
	class AWeapon* SpawnDefaultWeapon();

	/**무기 장착 */
	void EquipWeapon(AWeapon* WeaponToEquip,bool bSwapping = false);

	/**무기를 떨어트림 */
	void DropWeapon();

	/**현재 떨어진 무기를 장착 */
	void SwapWeapon(AWeapon* WeaponToSwap);

	/**총알 맵 ,총알값 초기화  */
	void InitializeAmmoMap();

	/**총이 총알을 갖고있는가 */
	bool WeaponHasAmmo();

	void PlayFireSound();

	void SendBullet();

	void PlayGunFireMontage();

	//장착된 무기 종류의 탄약이 있는지
	bool CaaryinhAmmo();

	//서거나 앉을때 캡슐 높이 보간 
	void InterpCapsuleHalfHeight(float DeltaTime);

	void Aim();
	void StopAiming();
	
	void PickupAmmo(class AAmmo* Ammo);

	void InitializeInterpLocation();

	UFUNCTION(BlueprintCallable)
	int32 GetEmptyInventorySlot();

	void HighlightInventorySlot();

	UFUNCTION(BlueprintCallable)
	EPhysicalSurface GetSurfaceType();

	UFUNCTION(BlueprintCallable)
	void EndStun();

	UFUNCTION(BlueprintCallable)
	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

private:
	void InitializeInputSystem();
	void InitializeCameraFOV();
	void EquipDefaultWeapon();
	void ResetPicukSoundTimer();
	void ResetEquipSoundTimer();
	void MarkCharacterAsDead(AController* EventInstigator);
	void ReloadAmmoForEquippedWeapon();
	void SwapWeapons(int32 CurrentItemIndex, int32 NewItemIndex);
	void HandleTraceHitResult(const FHitResult& ItemTraceResult);
	void HandlePickupWidgetAndCustomDepth();
	void HandleLastFrameTraceItem();
	void ClearLastFrameTraceItem();
	bool CanSwapWeapons(AWeapon* WeaponToSwap) const;
	void UpdateInventorySlot(AWeapon* WeaponToSwap);
	void SpawnMuzzleFlash(const FTransform& SocketTransform);
	void HandleBulletHitResult(FHitResult& HitResult);

	void SpawnBeamParticle(const FTransform& SocketTransform, const FVector& BeamEndLocation);
	//에니메이션 블루프린트가 클립을 잡으면 실행시키는 notify
	UFUNCTION(BlueprintCallable)
	void GrabClip();

	//에니메이션 블루프린트가 클립을 재장착 실행시키는 notify
	UFUNCTION(BlueprintCallable)
	void ReleaseClip();

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

	/** 히트지점 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticle;

	/** 연기흔적 파티클 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BeamParticle;
	
	/** 총쏘는 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* FireMontage;

	/** 재장전 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* ReloadMontage;

	/** 무기교체 몽타주 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* EquipMontage;

	/** 조준하고 있냐 참거짓*/
	UPROPERTY(visibleAnywhere, BlueprintReadOnly,Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bAiming = false;

	/** 기본 카메라 화면 뷰 값*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraDefaultFOV = 0;

	/** 카메라 확대했을때 줌 값*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraZoomedFOV = 35.f;

	/** 현재 시야*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float CameraCurrentFOV = 0.f;

	/** 조준 확대 속도*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float ZoomInterpSpeed =20.f;

	/**크로스헤어 퍼짐을 결정 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairSpreadMultiplire = 0.f;

	/**속도 구성요소 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairVelocityFactor = 0.f;

	/**공중에 있을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairInAirFactor = 0.f;

	/**조준 했을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairAimFactor =0.f;

	/**총쏘고 있을때 크로스헤어 퍼짐 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Crosshairs, meta = (AllowPrivateAccess = "true"))
	float CrosshairShootingFactor =0.f;

	/** 왼쪽 마우스 버튼 누름*/
	bool bFireButtonPressed = false;

	/*발사할 때 트루  타이머 기다릴때 거짓**/
	bool bShouldFire = true;

	/** 총쏘는 사이 타이머*/
	FTimerHandle AutoFireTimer;

	//총알 발사 타이머 변수
	float ShootTimeDuration = 0.05f;
	bool bFiringBullet = false;
	FTimerHandle CrosshairShootTimer;

	/** 아이템 추적된게 있나*/
	bool bShouldTraceForItems = false;

	/** 겹친 아이템 갯수*/
	int8 OverlappedItemCount;

	/** 마지막에 겹친 아이템*/
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Items, meta =(AllowPrivateAccess = "true"))
	class AItem* TraceHitItemLastFrame;

	/** 현재 장착 무기*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	class AWeapon* EquipeedWeapon;

	/** 웹폰 클래스*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;

	/** 현재 라인트레이서가 가르키는 아이템(null일수도 있음)*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	AItem* TraceHitItem;

	/** 카메라 앞에 거리 보간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpDistance = 250.f;

	/** 카메라 위에 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CameraInterpElevation = 65.f;

	/** Map 선언 EAmmoType에 따라 총알이 다름 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;

	/** 9mm 처음 총알 갯수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 Starting9MMAmmo = 85;

	/** AR 처음 총알 갯수 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 StartingARAmmo = 120;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	ECombatState CombatState = ECombatState::ECS_Unoccupied;

	/**탄약 트렌스폼 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	FTransform ClipTransForm;

	/**재장전 하든동안 캐릭터 손에 부착할 신 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	USceneComponent* HandSceneCOmponet;

	/**  웅크리면 진실 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	bool bCrouching = false;

	/**  기본 걷기 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float BaseMovementSpeed = 600.f;

	/**  앉았을때 걷기 속도 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	float CrouchMovementSpeed = 250.f;

	/**현재 캡슐 높이 */
	float CurrentCapsulHalfHeight= 88.f;

	/**서있을때 캡슐 높이 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float StandingCapsuleHalfHeight = 88.f;

	/**앉았을때 캡슐 half높이 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchCapsulHalfHeight =44.f;

	/**기본 멈출때 미끄러지는정도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float BaseGroundFriction =2.f;

	/** 앉았을 때 멈출때 미끄러지는정도 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float CrouchGroundFriction  = 100.f;

	/** 우클릭 버튼을 눌렀나*/
	bool bAimingButtonPressed = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* WeaponInterpComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* InterpComp6;

	// interp 위치 배얄 구조체
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	TArray<FInterpLocation> InterpLocation;

	FTimerHandle PickupSoundTimer;
	FTimerHandle EqiupSoundTimer;

	bool bShouldPlayPickupSound = true;
	bool bShouldPlayEquipSound = true;

	//** 픽업 사운드 재생할때 기다려야하는 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float PicupSoundResetTime =.2f;
	//** 장착 재생할때 기다려야하는 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	float EquipSOundResetTime = .2f;

	//인벤토리 배열
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = Inventory , meta = (AllowPrivateAccess = "true"))
	TArray<AItem*> Inventory;

	const int INVENTORY_CAPACITY = 6;

	/** 인벤토리에 보내는 위임자*/
	UPROPERTY(BlueprintAssignable,Category=Delegate,  meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;

	UPROPERTY(BlueprintAssignable, Category = Delegate, meta = (AllowPrivateAccess = "true"))
	FHighlightIconDelegate HighlightIconDelegate;

	/** 현재강조된 엔덱스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Inventory, meta = (AllowPrivateAccess = "true"))
	int32 HighlightedSlot =-1;

	/** 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health =100.f;

	/** 최대체력 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	/** 맞을 때 사운드 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundBase* MeleeImpactSound;

	/** 맞을 때 피 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* BloodParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;

	/** 스턴 걸릴 확률 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float StunChance =.25f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* DeathMontage;

	//캐릭터가 죽으면 트루
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bDead = false;
public:
	FORCEINLINE bool GetAiming() const { return bAiming; }
	FORCEINLINE class UCameraComponent* GetCamera() const { return Camera; }
	FORCEINLINE int8 GetOverlappedItemCOunt() const { return OverlappedItemCount; }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetCrouching() const { return bCrouching; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquipeedWeapon; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHelath() const { return MaxHealth; }
	FORCEINLINE USoundBase* GetMeleeImpactSound() const { return MeleeImpactSound; }
	FORCEINLINE UParticleSystem* GetBloodParticle() const { return BloodParticle; }
	FORCEINLINE float GetStunChance() const { return StunChance; }
	FORCEINLINE bool ShouldPlayPickupSound() const { return bShouldPlayPickupSound; }
	FORCEINLINE bool ShouldPlayEquipSound() const { return bShouldPlayEquipSound; }

	UFUNCTION(BlueprintCallable)
	float GetCrosshairSpreadMultiplier() const;

	/** 겹친 아이템 갯수를 더하거나 뺴준다*/
	void IncrementOverlappedItemCount(int8 Amount);
	void GetPickupItem(class AItem* Item);
	void IncrementInterpLocItemCount(int32 Index, int32 Amount);
	void StartPickupSoundTimer();
	void StartEquipSoundTimer();
	void UnHighlightInventorySlot();
	void Stun();

	/** 카메라 앞에 보여줄 무기 위치*/
	FVector GetCameraInterpLocation();
	FInterpLocation GetInterpLocation(int32 Index);
	int32 GetInterpLocationIndex();

};
