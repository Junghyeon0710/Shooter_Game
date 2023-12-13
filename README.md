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
#### 움직이거나 쏘면 크로스헤어 반동을 주기 위해 4개의 Texture을 사용했습니다.
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

## CharacterAnimInstnace

## PlayerController

## Item

## Enemy
