// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UENUM(BlueprintType)
enum class EItemRariy : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_Max UMETA(DisplayName = "DefaultMax")

};

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Falling UMETA(DisplayName = "Falling"),
	

	EIS_Max UMETA(DisplayName = "DefaultMax")

};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),

	EIT_Max UMETA(DisplayName = "DefaultMax"),
};

UCLASS()
class SHOOTER_GAME_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/**별의 갯수를 정하는 함수 */
	void SetActiveStars();

	/**아이템 속성 설정 */
	virtual void SetItemProperties(EItemState State);

	/**보간이 끝나면 부르는 함수 */
	void FinishInterping();

	/**아이템 보간 */
	void ItemInterp(float DeltaTime);

	/** 아이템 타입에 따른 보간 위치 가져오기*/
	FVector GetInterpLocation();

	void PlayPickupSound();
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="Item",meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	/** 플에어아가 아이템을 볼떄 뜨는 위젯*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickuWidget;

	/** 플에어아가 스피어에 닿으면 위젯 표시*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class USphereComponent* AreaSphere;

	/** 픽업 위젯에 표시되는 아이템 이름*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FString ItemName="Default";

	/** 픽업 위젯에 표시되는 총알갯수*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	int32 ItemCount = 0;

	/** 별의 갯수 - 위젯에 별의 갯수를 결정*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	EItemRariy ItemRarity = EItemRariy::EIR_Common;

	/** 별의 갯수 - 위젯에 별의 갯수를 설정*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	TArray<bool> ActiveStars;

	/** 아이템 상태*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	EItemState ItemState = EItemState::EIS_Pickup;

	/** 에셋이 사용할 Z위치에 대해 사용되는 곡선*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class UCurveFloat* ItemZCurve;

	/** 보간이 시작할때 시작 위치*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	FVector ItemInterpStartLocation;

	/** 카메라 앞의 대상 위치*/
	FVector CameraTargetLocation;

	/**보간을 했나*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	bool bInterping = false;

	/**보간이 시작되면 실행하는 타이머 */
	FTimerHandle ItemInterpTimer;
	/**보간 타이머 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	float ZCurveTime =0.7f;

	/** 캐릭터 클래스 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class AMyCharacter* Character;

	

	/** X,Y 보간할때 */
	float ItemInterpX = 0.f;
	float ItmeInterpY =0.f;

	//초기 Yaw 위치 보간할때
	float InterpInitialYawOffset = 0.f;
	 
	//보간 시작할떄 커브 값의 사용할 커브
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ItemScaleCurve;

	// 무기 주을떄 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class USoundBase* PickupSound;

	// 장착 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	class USoundBase* EquipSound;

	/* 아이템 타입 열거형 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item", meta = (AllowPrivateAccess = "true"))
	EItemType ItemType = EItemType::EIT_Max;

	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Items, meta = (AllowPrivateAccess = "true"))
	int32 InterpLocIndex = 0;

public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickuWidget; }
	FORCEINLINE USphereComponent* GetAreaSphere() const { return AreaSphere; }
	FORCEINLINE UBoxComponent* GetCollisionBox() const { return CollisionBox; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	void SetItemState(EItemState State);
	FORCEINLINE USkeletalMeshComponent* GetItemMesh() const { return ItemMesh; }
	FORCEINLINE USoundBase* GetPickupSound() const { return PickupSound; }
	FORCEINLINE USoundBase* GetEquipSound() const { return EquipSound; }
	FORCEINLINE int32 GetItemCount() const { return ItemCount; }

	/**내 캐릭터 클라스를 부름 */
	void StartItemCurve(AMyCharacter* Char);
	void PlayEquipSound();
};
