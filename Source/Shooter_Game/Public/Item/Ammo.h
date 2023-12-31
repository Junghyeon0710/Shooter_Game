// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "../AmmoType.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_GAME_API AAmmo : public AItem
{
	GENERATED_BODY()
public:
	AAmmo(); 

protected:
	virtual void BeginPlay() override;

	/**아이템 속성 설정 */
	virtual void SetItemProperties(EItemState State) override;

	UFUNCTION()
	void AmmoSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


private:

	//탄약 매쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = Ammo,meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* AmmoMesh;

	/** 탄약 유형*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	EAmmoType AmmoType;

	/** 총알갯수 texture*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	UTexture2D* AmmoIconTexture;

	/** 탄약을 줍기위한 구 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Ammo, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AmmoCollisionSphere;

public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }

	virtual void EnableCustomDepth() override;
	virtual void DisableCustomDepth() override;
};
