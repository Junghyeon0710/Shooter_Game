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

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	/**아이템 속성 설정 */
	virtual void SetItemProperties(EItemState State) override;
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
public:
	FORCEINLINE UStaticMeshComponent* GetAmmoMesh() const { return AmmoMesh; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
};
