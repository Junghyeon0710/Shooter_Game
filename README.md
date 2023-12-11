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
- ReceiveDrawHUD이벤트를 실행후 뷰포트의 중간값을 저장합니다.
![캡처](https://github.com/Junghyeon0710/TangTang/assets/133496610/243e617f-e876-4d33-bc68-385837530d04)
#### 클릭하시면 확대하실 수 있습니다.

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
#### 상태를 하나하나 따지며 값을 보간하며 더한 후 블루프린트 위에  HUD에서 CrosshairSpreadMultiplire 값을 더해줍니다.

## CharacterAnimInstnace

## PlayerController

## Item

## Enemy
