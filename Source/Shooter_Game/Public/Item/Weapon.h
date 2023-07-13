// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "../AmmoType.h"
#include "Engine/DataTable.h"
#include "../WeaponType.h"
#include "Weapon.generated.h"


USTRUCT(BlueprintType)
struct FWeaponDataTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 WeaponAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int MagazingCapacity;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* PickupSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class USoundBase* EquipSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* InventoryIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* AmmoIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 MaterialIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ClipBoneName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ReloadMontageSection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UAnimInstance> AnimBp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsBottom;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* CrosshairsTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AutoFireRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UParticleSystem* MuzzleFalsh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName BoneToHide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAutomatic;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HeadShotDamage;
	
};

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

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void BeginPlay() override;

	void FinishMovingSlide();
	void UpdateSlideDisplacement();
private:
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime = 0.7f;
	bool bFalling=false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category = "weapon",meta=(AllowprivateAccess="true"))
	int32 Ammo = 30;

	/** 탄약 최대치 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	int32 MagzineCapacity =30;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	EWeaponType WeaponType = EWeaponType::EWT_SubmachineGun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	EAmmoType AmmoType = EAmmoType::EAT_9mm;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	FName ReloadMontageSecion = "Reload";

	/**클립을 이동할 떄 트루  */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	bool bMovingClip = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "weapon", meta = (AllowprivateAccess = "true"))
	FName ClipBoneName = "smg_clip";

	UPROPERTY(EditDefaultsOnly,Category = DataTable, meta = (AllowprivateAccess = "true"))
	UDataTable* WeaponDataTable;

	int32 PreviousMaterialIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	UTexture2D* CrosshairsMiddle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	UTexture2D* CrosshairsLeft;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	UTexture2D* CrosshairsRight;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	UTexture2D* CrosshairsBottom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	UTexture2D* CrosshairsTop;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	float AutoFireRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	class UParticleSystem* MuzzleFalsh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	USoundBase* FireSound;

	/** 본 숨기기*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = DataTable, meta = (AllowprivateAccess = "true"))
	FName BoneToHide;

	/** 발사중에 슬라이드가 뒤로 밀리는 거리 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	float SlideDisplacement = 0.f;

	/** 커브 곡선*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	UCurveFloat* SlideDisplacementCurve;

	/* 사격중 업데이트 타이머**/
	FTimerHandle SlideTimer;

	// 권총 사격중 슬라이드를 옮길 시간
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	float SlideDisplacementTime= 0.5f;

	//권총이 사격할떄 움직였나
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	bool bMovingSlide = false;

	//권총 슬라이드 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	float MaxSlideDisplacement =4.f;

	/** 발사중에 총 회전 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	float RecoilRotation = 0.f;

	//권총 반동 회전 최대
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Pistol, meta = (AllowprivateAccess = "true"))
	float MaxRecoilRotation  = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wepaon Properties", meta = (AllowprivateAccess = "true"))
	bool bAutomatic = true; 

	/** 무기 데미지*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wepaon Properties", meta = (AllowprivateAccess = "true"))
	float Damage;

	/** 헤드 데미지*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wepaon Properties", meta = (AllowprivateAccess = "true"))
	float HeadShotDamae;
public:
	void ThrowWeapon();
	FORCEINLINE int32 GetAmmo() const { return Ammo; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagzineCapacity; }
	//총을 쏠때 캐릭터 클래스를 부름 ,총알 감소
	void DecremntAmmo();

	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	void SetWeaponType(EWeaponType Weapon) { WeaponType = Weapon; }

	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadMontageSecion() const { return ReloadMontageSecion; }
	FORCEINLINE void SetReloadMontageSecion(FName Name) { ReloadMontageSecion = Name; }
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }
	FORCEINLINE float GetAutoFireRate() const { return AutoFireRate; }
	FORCEINLINE UParticleSystem* GetMuzzleFlash() const { return MuzzleFalsh; }
	FORCEINLINE USoundBase* GetFireSound() const { return FireSound; }
	FORCEINLINE bool GetAutomatic() const { return bAutomatic; }
	FORCEINLINE float GetDamage() const {return Damage; }
	FORCEINLINE float GetHeadShotDamage() const { return HeadShotDamae; }
		
	void StartSlideTimer();
	void ReloadAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }

	bool ClipIsFull();
};
