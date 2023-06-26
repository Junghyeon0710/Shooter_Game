// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "../AmmoType.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_Max UMETA(DisplayName = "DefaultMax")
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
	FORCEINLINE FName GetClipBoneName() const { return ClipBoneName; }

	void ReloadAmmo(int32 Amount);

	FORCEINLINE void SetMovingClip(bool Move) { bMovingClip = Move; }
};
