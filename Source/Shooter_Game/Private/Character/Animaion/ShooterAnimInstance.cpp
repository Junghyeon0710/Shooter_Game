// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animaion/ShooterAnimInstance.h"
#include <../Public/Character/MyCharacter.h>
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/KismetMathLibrary.h"

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
	}
	if (ShooterCharacter)
	{
		bCrouching = ShooterCharacter->GetCrouching();
		bReloading = ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		FVector Veloctiy = ShooterCharacter->GetVelocity();
		Veloctiy.Z = 0;
		Speed = Veloctiy.Size();

		//캐릭터가 공중에 있나
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();

		//캐릭터가 움직이나
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			//업데이트할 때마다 입력 벡터에서 계산된 현재 가속도를 반환합니다.

			bIsAccelerating = true;
		}
		else
		{
			bIsAccelerating = false;
		}

		//Pawn의 조준 회전을 반환합니다.
		FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();

		//x축의 방향을 빌드
		FRotator MovementRotation = 
			UKismetMathLibrary::MakeRotFromX(
			ShooterCharacter->GetVelocity()); //캐릭터 속도

		//속도에서 회전을 빼주고 정규화
		MoventOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(
			MovementRotation,
			AimRotation).Yaw;
		if (ShooterCharacter->GetVelocity().Size() > 0.f)
		{
			LastMoventOffsetYaw = MoventOffsetYaw;
		}
	

	/*	if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, FString::Printf(TEXT("Base Aim Rotation: %f"), MoventOffsetYaw));
		}*/

		bAiming = ShooterCharacter->GetAiming();

		if (bReloading)
		{
			OffsetState = EOffsetState::EOS_Reloading;
		}
		else if(bIsInAir)
		{
			OffsetState = EOffsetState::EOS_InAir;
		}
		else if (ShooterCharacter->GetAiming())
		{
			OffsetState = EOffsetState::EOS_Aiming;
		}
		else
		{
			OffsetState = EOffsetState::EOS_Hip;
		}
	}
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

	if (Speed > 0 || bIsInAir)
	{
		
		// 돌 필요가 없다
		RootYawOffset = 0.f;
		TICCharacaterYaw = ShooterCharacter->GetActorRotation().Yaw;
		TICCharacterYawLastFrame = TICCharacaterYaw;
		RotationCurveValueLastFrame = 0.f;
		RotationCurve = 0.f;
	}
	else
	{
		TICCharacterYawLastFrame = TICCharacaterYaw;
		TICCharacaterYaw = ShooterCharacter->GetActorRotation().Yaw;
		const float TIPYawDelta = TICCharacaterYaw - TICCharacterYawLastFrame;

		//루트 yaw offset, -180 , 180 사이
		// 각도를 (-180, 180] 범위로 고정합니다. (NormalizeAxis)
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIPYawDelta);

		const float Turning = GetCurveValue(TEXT("Turning"));
		if (Turning > 0)
		{
			bTurningPlace = true;

			RotationCurveValueLastFrame = RotationCurve;
			RotationCurve = GetCurveValue(TEXT("Rotation"));
			const float DeltaRotiation = RotationCurve - RotationCurveValueLastFrame;

			// RootYawOffset > 0 -> 왼쪽으로 회전 , RootYawOffset <0, ->오른쪽으로 회전
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotiation : RootYawOffset += DeltaRotiation;

			const float ABSRootyawOffset = FMath::Abs(RootYawOffset);


			if (ABSRootyawOffset > 90.f)
			{
				const float YawExcess = ABSRootyawOffset - 90.f;
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			bTurningPlace = false;
		}
	}
		if (bTurningPlace)
		{
			if (bReloading)
			{
				RecoilWeight = 1.f;
			}
			else
			{
				RecoilWeight = 0.f;
			}
		}
		else // 안 돌고 있을때
		{
			if (bCrouching)
			{
				if (bReloading)
				{
					RecoilWeight = 1.f;
				}
				else
				{
					RecoilWeight = 0.1f;
				}

			}
			else
			{
				if (bAiming || bReloading)
				{
					RecoilWeight = 1.f;
				}
				else
				{
					RecoilWeight = 0.5f;
				}
			}
		}
	
	
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
