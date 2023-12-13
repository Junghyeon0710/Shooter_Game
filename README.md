## Unreal5 : Shooter Game
포트폴리오 목적으로 제작된 간단한 FPS게임입니다.

- 엔진 : Unreal Engine 5.3
- IDE : Visual Studio 2022
- 툴 : C++ , Blueprint 혼합사용
- 인원 : 개인 개발
- 개발기간 : 두달 내외

## 영상
[![Video Label](https://img.youtube.com/vi/PjArJyTwDXk/0.jpg)](https://www.youtube.com/watch?v=PjArJyTwDXk')
## Character
- 기본 움직임
- SetupPlayerInputComponent(입력키 바인딩)
- TakeDamage(데미지 처리)
- HUD(크로스헤어)
- 인벤토리 슬롯 관리

>HUD
![image](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/89052df8-9d49-4581-8f07-078b6d81432d)
![캡처](https://github.com/Junghyeon0710/TangTang/assets/133496610/243e617f-e876-4d33-bc68-385837530d04)
#### 움직이거나 쏘면 크로스헤어 반동을 주기 위해 하나의 위젯이 아닌 4개의 Texture을 사용했습니다.
###### 클릭하시면 확대하실 수 있습니다.


> 크로스헤어 Spread 계산
```C++
void AMyCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeddRange(0.f, 600.f);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;

	//입력값이 0,600 아웃값에 0,1 값이 속도면 속도에 비율을 아웃값에 맞춰서 반환
	// 값이 만약 150이면 입력값의 4분의 1이므로 출력값으로 매핑해서 하면 0.25가 나옴 출력값에서 클램팽됨
	//[Input:Range] inclusive로 고정된 주어진 값에 대해 [Output:Range] Inclusive에서 해당 백분율을 반환합니다.
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeddRange, VelocityMultiplierRange, Velocity.Size());

	// 공중에 있을떄 크로스 헤어
	if (GetCharacterMovement()->IsFalling()) //공중에 있나
	{
		//천천히 크로스헤어가 퍼짐
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 1, DeltaTime, 2.25f);
	}
	else //바닥에 닿았으면
	{
		//크로스헤어를 빠르게 좁힘
		CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	//크로스헤어 조준중일때 계산

	if (bAiming) //조준중인가
	{
		//십자선 축소
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.6f, DeltaTime, 30.f);
	}
	else // 조준중이 아니면
	{
		//십자선 복구
		CrosshairAimFactor = FMath::FInterpTo(CrosshairAimFactor, 0.f, DeltaTime, 30.f);
	}

	//0.05초뒤에 참
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor, 0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplire = 0.5f + CrosshairVelocityFactor + CrosshairInAirFactor - CrosshairAimFactor + CrosshairShootingFactor; //값을 다 더한 후 블루프린트에서 처리
}
```
#### 캐릭터 상태를 하나하나 따지며 값을 보간하며 더한 후 블루프린트 위에 HUD에서 CrosshairSpreadMultiplire 값을 더해줍니다. Tick함수에서 호출되므로 실시간으로 크로스헤어를 조정할 수 있습니다.

>인벤토리
- 블루프린트에서 함수 바인딩을 통해서 맞는 Image를 넣도록 했습니다.
- 아래는 하나의 예시와 방법 입니다.
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/506af371-ecfa-412e-8a8b-9036b1dbe0cb)
#### 외형 브러시를 함수에 바인딩 합니다.
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/56cf19bd-5d16-4720-974b-e4c01c1c4746)
#### 함수는 각 슬롯마다 인덱스를 가지고 있는데 해당 슬롯 인덱스에 맞는 무기의 정보를 가져와 외형을 변하게 하는 함수입니다.
>인벤토리 애니메이션
- 캐릭터가 들고 있는 해당 무기의 인벤토리칸 테두리를 살짝 올려 몇번 무기를 들고있는지 애니메이션을 줬습니다.
- 캐릭터가 LineTrace로 무기를 보고있을 때 몇번 인벤토리 슬롯에 들어갈지 알려주는 하이라이트 애니메이션을 줬습니다.
- 아래는 하나의 예시와 방법입니다.
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/fff1ca2b-04bd-41cf-92d0-23520af102e5)
#### 들고있는 무기를 알려주는 장착 애니메이션과 들어갈 인벤토리 슬롯을 알려주는 하이라이트 애니메이션 적용모습입니다.
#### 아래는 장착 애니메이션 방법입니다.
```C++
Character.h
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

	UPROPERTY(BlueprintAssignable,Category=Delegate,  meta = (AllowPrivateAccess = "true"))
	FEquipItemDelegate EquipItemDelegate;
```
- 먼저 다이나믹 델리게이트를 선언한 후 UPROPERTY매크로에서 블루트린에서 델리게이트를 할당 할 수 있게 하는 BlueprintAssignable를 추가시켜줍니다.
- 인자값으로는 현재 들고있는 무기의 Slot인덱스와 다음무기 Slot인덱스를 가집니다.
> 무기장착 함수입니다.
```C++
void AMyCharacter::EquipWeapon(AWeapon* WeaponToEquip, bool bSwapping)
{
	if (WeaponToEquip)
	{
		//핸드 소켓 얻기
		const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			//무기를 오른쪽 소켓에 붙임
			HandSocket->AttachActor(WeaponToEquip, GetMesh());
		}

		if (EquipeedWeapon == nullptr)
		{
			//장착된 아이넴이 없다.
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if (!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquipeedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
		}

		//장착한 무기는 스폰된 무기
		EquipeedWeapon = WeaponToEquip;
		//장착한 상태
		EquipeedWeapon->SetItemState(EItemState::EIS_Equipped);
	}
}
```
- 해당 델리게이트에 Broadcast를 통해 값을 넣어줍니다.
- 넣어준 후 아래 블루트린트에서 진행했습니다.
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/2fd94655-ce10-4cfb-aef8-346996eac278)
- 해당 델리게이트에 함수를 바인딩했습니다
- 함수는 간단하게 무기를 장착하면 현재 무기의 적용되있는 애니메이션을 Reverse Animation해서 원래 위치로 돌려줍니다.
- 그런다음에 장착된 무기에는 애니메이션을 플레이해서 장착된 무기를 알려주는 함수입니다.

## CharterAnim
- Epic Games Animation Starter Pack사용
- Belica Animation 사용
- Lern(달릴떄 기울기),TurnInPlace(제자리에서 일정 Yaw넘으면 제자리턴)기능 구현

##### 위에서 아래로 왼쪽에서 오른쪽 순서입니다.
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/46a7b5d1-b706-4f27-b5c4-e358fc291714)
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/904d3697-9deb-4f19-8cca-c39d6d8ec2d6)
![캡처1](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/4042df09-01f5-4967-9f8d-896316088a97)
![캡처2](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/b423a480-66d7-4507-849b-8288324cb8a5)
![캡처3](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/e4691676-f07c-4a09-8945-20b2b663ef36)
#### AimOffset,BlendSpace,Layered blend per bone,FABRIK 사용했습니다.

## Item
#### 무기 총알등 Item 무스 클래스
- 등급결정(별갯수)
- CustomDepth 설정

>등급결정
```C++
void AItem::SetActiveStars()
{
	for (int32 i = 0; i <= 5; i++)
	{
		ActiveStars.Add(false);
	}

	switch (ItemRarity)
	{
	case EItemRariy::EIR_Damaged:
		ActiveStars[1] = true;
		break;
	case EItemRariy::EIR_Common:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		break;
	case EItemRariy::EIR_Uncommon:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		break;
	case EItemRariy::EIR_Rare:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		break;
	case EItemRariy::EIR_Legendary:
		ActiveStars[1] = true;
		ActiveStars[2] = true;
		ActiveStars[3] = true;
		ActiveStars[4] = true;
		ActiveStars[5] = true;
		break;
	case EItemRariy::EIR_Max:
		break;
	default:
		break;
	}
}
```
- 맞는 방법이 있지만 저는 간단하게 Enum으로 등급을 만들고 bool 배열 ActiveStars만들었습니다.
- 그 다음 별 갯수만큼 true를 넣어주는 방식으로 했습니다.
- 그런다음 데이터 테이블을 만든 뒤 OnConstruction함수에서 실행하여 등급이 별경될 때 마다 바뀐 정보를 바로 적용시키게 구현했습니다.
>데이터 테이블 설정
```C++
void AItem::InitializeFromRarityTable()
{
	//아이템 데이터 테이블 읽어옴

	//아이템 데이터 테이블 경로 
	FString RarityTablePath(TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_ItemRarityDataTable.DT_ItemRarityDataTable'"));
	UDataTable* RarityTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(),
		nullptr, *RarityTablePath));

	if (RarityTableObject)
	{
		FItemRarityTable* RarityRow = nullptr;
		switch (ItemRarity)
		{
		case EItemRariy::EIR_Damaged:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Damaged"), TEXT(""));
			break;
		case EItemRariy::EIR_Common:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Common"), TEXT(""));
			break;
		case EItemRariy::EIR_Uncommon:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Uncommon"), TEXT(""));
			break;
		case EItemRariy::EIR_Rare:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Rare"), TEXT(""));
			break;
		case EItemRariy::EIR_Legendary:
			RarityRow = RarityTableObject->FindRow<FItemRarityTable>(FName("Legendary"), TEXT(""));
			break;

		}
		if (RarityRow)
		{
			GlowColor = RarityRow->GlowColor;
			LightColor = RarityRow->LightColor;
			DarkColor = RarityRow->DarkColor;
			NumberOfStars = RarityRow->NumberOfStars;
			IconBackgorund = RarityRow->IconBackground;
			if (GetItemMesh())
			{
				GetItemMesh()->SetCustomDepthStencilValue(RarityRow->CustomDepthStecil);
			}
		}
	}
}
```
#### 이 함수는 OnConstruction함수에서 실행되며 언리얼 에디터에서 엑터의 속성이나 트랜스폼 정보가 변경될 때 호출되는 함수이므로 아이템의 등급이 변경될때마다 테이블 정보를 가져와 설정할 수 있습니다.

## Weapon
- 데이터 테이블을 이용한 데이터 관리
>Weapon.h
```C++
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

```

```C++
void AWeapon::InitializeFromWeaponTable()
{
	const FString WeaponTablePath = TEXT("/Script/Engine.DataTable'/Game/DataTable/DT_WeaponDataTable.DT_WeaponDataTable'");
	UDataTable* WeaponTableObject = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, *WeaponTablePath));

	if (WeaponTableObject)
	{
		FWeaponDataTable* WeaponDataRow = nullptr;
		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("SubMachineGun"), TEXT(""));
			break;
		case EWeaponType::EWT_AssaultRifle:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("AssaultRifle"), TEXT(""));
			break;
		case EWeaponType::EWT_Pistol:
			WeaponDataRow = WeaponTableObject->FindRow<FWeaponDataTable>(FName("Pistol"), TEXT(""));

		default:
			break;
		}
		if (WeaponDataRow)
		{
			AmmoType = WeaponDataRow->AmmoType;
			Ammo = WeaponDataRow->WeaponAmmo;
			MagzineCapacity = WeaponDataRow->MagazingCapacity;
			SetPickupSound(WeaponDataRow->PickupSound);
			SetEquipSound(WeaponDataRow->EquipSound);
			GetItemMesh()->SetSkeletalMesh(WeaponDataRow->ItemMesh);
			SetItemName(WeaponDataRow->ItemName);
			SetIconItem(WeaponDataRow->InventoryIcon);
			SetAmmoIcon(WeaponDataRow->AmmoIcon);

			SetMaterialInstance(WeaponDataRow->MaterialInstance);
			PreviousMaterialIndex = GetMaterialIndex();
			GetItemMesh()->SetMaterial(PreviousMaterialIndex, nullptr);
			SetMaterialIndex(WeaponDataRow->MaterialIndex);

			ClipBoneName = WeaponDataRow->ClipBoneName;
			SetReloadMontageSecion(WeaponDataRow->ReloadMontageSection);
			GetItemMesh()->SetAnimInstanceClass(WeaponDataRow->AnimBp);

			CrosshairsMiddle = WeaponDataRow->CrosshairsMiddle;
			CrosshairsLeft = WeaponDataRow->CrosshairsLeft;
			CrosshairsRight = WeaponDataRow->CrosshairsRight;
			CrosshairsTop = WeaponDataRow->CrosshairsTop;
			CrosshairsBottom = WeaponDataRow->CrosshairsBottom;
			AutoFireRate = WeaponDataRow->AutoFireRate;
			MuzzleFalsh = WeaponDataRow->MuzzleFalsh;
			FireSound = WeaponDataRow->FireSound;
			BoneToHide = WeaponDataRow->BoneToHide;
			bAutomatic = WeaponDataRow->bAutomatic;

			Damage = WeaponDataRow->Damage;
			HeadShotDamae = WeaponDataRow->HeadShotDamage;

		}

		if (GetMaterialInstance())
		{
			SetDynamicMaterialInstance(UMaterialInstanceDynamic::Create(GetMaterialInstance(), this));
			GetDynamicMaterialInstance()->SetVectorParameterValue(TEXT("FresnelColor"), GetGlowColor());
			GetItemMesh()->SetMaterial(GetMaterialIndex(), GetDynamicMaterialInstance());
			EnableGlowMaterial();
		}
	}
}
```
>WeaponDataTable

![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/d6a638f1-2134-4347-9c5a-9f79545e60fa)


- WeaponTable에서 정보를 가져와 초기화 시켜줬습니다
- Item 클래스와 마찬가지로 OnConstruction함수에서 실행했습니다.

## Weapon AnimBp
- 게임안에서 무기 장전할 때 무기 클립이 가만히 있어서 게임이 현실감이 떨어졌습니다
- 그래서 캐릭터에 SecenComponet를 하나 만든 후 클립을 꺼내오고 뺴는 손인 왼쪽 손에 붙여줬습니다.
- 붙여준 후 캐릭터가 장전을 할 때 무기의 클립의 트랜스폼을 왼쪽손에 위치로 변경해 장전할 때 손으로 빼가고 다시 끼는것처럼 구현했습니다.

  
>Weapon AnimBP AnimGraph
![캡처](https://github.com/Junghyeon0710/Shooter_Game/assets/133496610/8fd9b51a-155e-498e-bda1-b5a91b9f58a5)

## Enemy
