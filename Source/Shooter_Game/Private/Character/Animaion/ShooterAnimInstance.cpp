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
	

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::White, FString::Printf(TEXT("Base Aim Rotation: %f"), MoventOffsetYaw));
		}

		bAiming = ShooterCharacter->GetAiming();
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AMyCharacter>(TryGetPawnOwner());
}
