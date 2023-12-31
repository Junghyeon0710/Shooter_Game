// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animaion/ShooterAnimInstance.h"
#include <../Public/Character/MyCharacter.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"
#include "../Public/Item/Weapon.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	// ShooterCharacter가 아직 할당되지 않았다면 할당합니다.
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		// 캐릭터 속성 업데이트
		UpdateCharacterProperties();

		// 회전 및 상체 동작 상태 업데이트
		UpdateRotationAndOffsetState();

		// 회전 속성 업데이트
		UpdateRotationProperties();
	}

	// 턴인플레이스와 기울임 동작 업데이트
	TurnInPlace();
	Lean(DeltaTime);
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) return;

	Pitch = ShooterCharacter->GetBaseAimRotation().Pitch;

	// 캐릭터가 움직이거나 공중에 있는 경우 회전 필요 없음
	if (CharacterSpeed > 0 || bIsInAir)
	{
		ResetRotation();
	}
	else
	{
		//회전 
		UpdateCharacterRotationAndCurveValues(ShooterCharacter,TICCharacterYawLastFrame,TICCharacaterYaw,RootYawOffset,bTurningPlace,RotationCurveValueLastFrame);
	}
	UpdateRecoilWeight();
}

void UShooterAnimInstance::Lean(float DeltaTime)
{
	if (ShooterCharacter == nullptr) return;

	CharacterYawLastFrame = CharacterYaw;
	CharacterYaw = ShooterCharacter->GetActorRotation();

	//정규화된 AB
	const FRotator Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterYaw, CharacterYawLastFrame);

	const float Target = Delta.Yaw / DeltaTime;
	const float Interp = FMath::FInterpTo(YawDelta, Target, DeltaTime, 6.f);

	YawDelta = FMath::Clamp(Interp, -90, 90);

}

void UShooterAnimInstance::UpdateCharacterProperties()
{
	// 캐릭터의 기본 동작, 조준 여부, 리로딩 상태, 장비 장착 상태, FABRIK 사용 여부를 업데이트합니다.
	bCrouching = ShooterCharacter->GetCrouching();
	bAiming = ShooterCharacter->GetAiming();
	bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
	bEquipping = ShooterCharacter->GetCombatState() == ECombatState::ECS_Equipping;
	bShoulUseFABRIK = ShooterCharacter->GetCombatState() == ECombatState::ECS_Unoccupied || ShooterCharacter->GetCombatState() == ECombatState::ECS_FireTimerInProgress;

	// 캐릭터 속도 업데이트
	FVector Velocity = ShooterCharacter->GetVelocity();
	Velocity.Z = 0;
	CharacterSpeed = Velocity.Size();

	// 캐릭터가 공중에 있는지 여부 업데이트
	bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

	// 캐릭터가 움직이는지 여부 업데이트
	bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
}

void UShooterAnimInstance::UpdateRotationAndOffsetState()
{
	// 캐릭터의 회전 및 상체 동작 상태 업데이트
	if (bReloading)
	{
		OffsetState = EOffsetState::EOS_Reloading;
	}
	else if (bIsInAir)
	{
		OffsetState = EOffsetState::EOS_InAir;
	}
	else if (bAiming)
	{
		OffsetState = EOffsetState::EOS_Aiming;
	}
	else
	{
		OffsetState = EOffsetState::EOS_Hip;
	}
}

void UShooterAnimInstance::UpdateRotationProperties()
{
	//Pawn의 조준 회전을 반환
	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	//x축의 방향을 빌드
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetVelocity()); //캐릭터 속도
	//속도에서 회전을 빼주고 정규화
	MoventOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	
	// 캐릭터 속도가 0보다 크면 마지막 회전 속성을 업데이트
	if (ShooterCharacter->GetVelocity().Size() > 0.f)
	{
		LastMoventOffsetYaw = MoventOffsetYaw;
	}
}

void UShooterAnimInstance::UpdateEquippedWeaponType()
{
	// 장착된 무기의 유형 업데이트
	if (ShooterCharacter->GetEquippedWeapon())
	{
		EquippedWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}
}

void UShooterAnimInstance::ResetRotation()
{
	// 캐릭터가 움직이지 않을 때 루트 yaw offset을 초기화
	RootYawOffset = 0.f;
	TICCharacaterYaw = ShooterCharacter->GetActorRotation().Yaw;
	TICCharacterYawLastFrame = TICCharacaterYaw;
	RotationCurveValueLastFrame = 0.f;
	RotationCurve = 0.f;
}

void UShooterAnimInstance::UpdateCharacterRotationAndCurveValues(AActor* Character, float& FCharacterYawLastFrame, float& CharacaterYaw, float& CharacterRootYawOffset, bool& bCharacterTurningPlace, float& CharacterRotationCurveValueLastFrame)
{
	FCharacterYawLastFrame = CharacaterYaw;
	CharacaterYaw = Character->GetActorRotation().Yaw;
	const float TIPYawDelta = CharacaterYaw - FCharacterYawLastFrame;

	//루트 yaw offset, -180 , 180 사이
	// 각도를 (-180, 180] 범위로 고정합니다. (NormalizeAxis)
	CharacterRootYawOffset = UKismetMathLibrary::NormalizeAxis(CharacterRootYawOffset - TIPYawDelta);

	const float Turning = GetCurveValue(TEXT("Turning"));
	if (Turning > 0)
	{
		bCharacterTurningPlace = true;

		CharacterRotationCurveValueLastFrame = RotationCurve;
		RotationCurve = GetCurveValue(TEXT("Rotation"));

		// DeltaRotation은 현재 프레임과 이전 프레임의 회전 커브 차이를 나타냄
		const float DeltaRotiation = RotationCurve - CharacterRotationCurveValueLastFrame;

		// RootYawOffset > 0 -> 왼쪽으로 회전 , RootYawOffset <0, ->오른쪽으로 회전
		CharacterRootYawOffset > 0 ? CharacterRootYawOffset -= DeltaRotiation : CharacterRootYawOffset += DeltaRotiation;

		// RootYawOffset의 절대값을 얻음
		const float ABSRootyawOffset = FMath::Abs(CharacterRootYawOffset);

		if (ABSRootyawOffset > 90.f)
		{
			// RootYawOffset이 90보다 크면 조정
			const float YawExcess = ABSRootyawOffset - 90.f;
			CharacterRootYawOffset > 0 ? CharacterRootYawOffset -= YawExcess : CharacterRootYawOffset += YawExcess;
		}
	}
	else
	{
		bCharacterTurningPlace = false;
	}
}

void UShooterAnimInstance::UpdateRecoilWeight()
{
	// 턴인플레이 중 또는 특정 상태에 따라 리코일 가중치 설정
	if (bTurningPlace)
	{
		RecoilWeight = (bReloading || bEquipping) ? 1.f : 0.f;
	}
	else
	{
		if (bCrouching)
		{
			RecoilWeight = (bReloading || bEquipping) ? 1.f : 0.1f;
		}
		else
		{
			RecoilWeight = (bAiming || bReloading || bEquipping) ? 1.f : 0.5f;
		}
	}
}
