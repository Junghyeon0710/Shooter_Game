 // Fill out your copyright notice in the Description page of Project Settings.


#include "Character/MyCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Engine/SkeletalMeshSocket.h"
#include <DrawDebugHelpers.h>
#include <Particles/ParticleSystemComponent.h>
#include <Item/Item.h>
#include "Components/WidgetComponent.h"
#include "Item/Weapon.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

// Sets default values
AMyCharacter::AMyCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 180.f;
	SpringArm->bUsePawnControlRotation = true; // 폰에 따라 회전
	SpringArm->SocketOffset = FVector(0.5f, 50.f, 70.f);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm,USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	// 컴토롤러가 회전할떄 카메라만 움직이기
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	//
	GetCharacterMovement()->bOrientRotationToMovement = false; // 캐릭터가 누른키 방향으로 움직임
	//GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); //회전율
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	HandSceneCOmponet = CreateDefaultSubobject<USceneComponent>(TEXT("HandSceneComp"));

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultContext, 0);
		}
	}

	if (SpringArm)
	{
		CameraDefaultFOV = Camera->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	//무기를 스폰하고 장착
	EquipWeapon(SpawnDefaultWeapon());

	InitializeAmmoMap();
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//카메라 줌 보간 
	CameraIntrerpZoom(DeltaTime); 
	// 조준했을때 화면 돌아가는거 속도 조절 
	SetLookRates();

	//크로스헤어 퍼짐 계산
	CalculateCrosshairSpread(DeltaTime);
	
	//스피어에 겹친 아이템이 있거나 0보다 많으면
	TraceForItems();
	
	
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AMyCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AMyCharacter::AimingButtonReleased);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AMyCharacter::SelectButtonPressed);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AMyCharacter::SelectButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyCharacter::ReloadButtonPressed);


	}
}

void AMyCharacter::GrabClip()
{
	if (EquipeedWeapon == nullptr || HandSceneCOmponet == nullptr) return;

	//탄약 인덱스
	int32 ClipBoneIndex = 
		EquipeedWeapon->GetItemMesh()->GetBoneIndex(EquipeedWeapon->GetClipBoneName());

	/** 잡기 시작할 떄 트랜스폼*/
	ClipTransForm = EquipeedWeapon->GetItemMesh()->GetBoneTransform(ClipBoneIndex);
	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::KeepRelative, true);
	HandSceneCOmponet->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("Hand_L")));
	HandSceneCOmponet->SetWorldTransform(ClipTransForm);

	EquipeedWeapon->SetMovingClip(true);

}

void AMyCharacter::ReleaseClip()
{
	EquipeedWeapon->SetMovingClip(false);
}

float AMyCharacter::GetCrosshairSpreadMultiplier() const
{
	return CrosshairSpreadMultiplire;
}

void AMyCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		OverlappedItemCount = 0;
		bShouldTraceForItems = false;
	}
	else
	{
		OverlappedItemCount += Amount;
		bShouldTraceForItems = true;
	}
}



void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		const FVector FowardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		AddMovementInput(FowardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	float TurnScaleFactor;
	float LookUpScaleFactor;
	if (bAiming)
	{
		TurnScaleFactor = MouseAimingTurnRate;
		LookUpScaleFactor = MouseAimingLookUpRate;
	}
	else
	{
		TurnScaleFactor = MouseHipTurnRate;
		LookUpScaleFactor = MouseHipLookUpRate;
	}
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X* TurnScaleFactor);
		AddControllerPitchInput(LookAxisVector.Y* LookUpScaleFactor);
	}
}

void AMyCharacter::Fire()
{
	if (EquipeedWeapon == nullptr) return;
	if (CombatState != ECombatState::ECS_Unoccupied) return;

	if (WeaponHasAmmo())
	{
		//사운드 재생
		PlayFireSound();
		//총알 발사
		SendBullet();
		//몽타주 재생
		PlayGunFireMontage();

		//총알 뺌
		EquipeedWeapon->DecremntAmmo();

		//총쏘면 에임 벌어짐
		StartCrosshairBulletFire();

		//타이머재생
		StartFireTimer();
	}

}

bool AMyCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
{
	FHitResult CrosshairHitResult;
	bool bCrooshairHit = TraceUnderCrosshairs(CrosshairHitResult, OutBeamLocation);

	if (bCrooshairHit)
	{
		OutBeamLocation = CrosshairHitResult.Location;
	}
	else
	{

	}

	//두번째 라인트레이스 총위치에서			
	FHitResult WeaponTraceHit;
	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation; //시작부터 끝에 거리는 끝에서 처음 거리를 뺴면된다.
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd *1.25;
	GetWorld()->LineTraceSingleByChannel(
		WeaponTraceHit,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (WeaponTraceHit.bBlockingHit) //// 총입구에서 발사지점 사이에 물체가 있다면 
	{
		OutBeamLocation = WeaponTraceHit.Location;
		return true;
	}
	return false;
	
}

void AMyCharacter::AimingButtonPressed()
{
	bAiming = true;
}

void AMyCharacter::AimingButtonReleased()
{
	bAiming = false;
}

void AMyCharacter::SelectButtonPressed()
{
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this);

		if (TraceHitItem->GetPickupSound())
		{
			UGameplayStatics::PlaySound2D(this, TraceHitItem->GetPickupSound());
		}
	}
	
} 

void AMyCharacter::SelectButtonReleased()
{
}

void AMyCharacter::ReloadButtonPressed()
{
	ReloadWeapon();
}

void AMyCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (EquipeedWeapon == nullptr) return;

	// 맞는 탄약이 있는지 체크
	if (CaaryinhAmmo())
	{
		CombatState = ECombatState::ECS_Reloading;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (ReloadMontage && AnimInstance)
		{
			AnimInstance->Montage_Play(ReloadMontage);
			AnimInstance->Montage_JumpToSection(
				EquipeedWeapon->GetReloadMontageSecion(), ReloadMontage);
		}
	}
}

void AMyCharacter::FinishReloading()
{

	CombatState = ECombatState::ECS_Unoccupied;
	if (EquipeedWeapon == nullptr) return;

	const auto AmmoType = EquipeedWeapon->GetAmmoType();
	if (AmmoMap.Contains(AmmoType))
	{
		//캐릭터가 가지고있는 장비 유형의 탄약
		int32 CarriedAmmo = AmmoMap[AmmoType];

		const int32 MagEmptySpace =
			EquipeedWeapon->GetMagazineCapacity() - 
			EquipeedWeapon->GetAmmo();

		if (MagEmptySpace > CarriedAmmo)
		{
			EquipeedWeapon->ReloadAmmo(CarriedAmmo);
			CarriedAmmo = 0;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
		else
		{
			EquipeedWeapon->ReloadAmmo(MagEmptySpace);
			CarriedAmmo -= MagEmptySpace;
			AmmoMap.Add(AmmoType, CarriedAmmo);
		}
	}
}

void AMyCharacter::CameraIntrerpZoom(float DeltaTime)
{
	// 현재 카메라 뷰를 설정
	if (bAiming) 	// 버튼을 눌렀나
	{
		//카메라 줌을 보강
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraZoomedFOV,
			DeltaTime,
			ZoomInterpSpeed);
	}
	else
	{
		//카메라 기본값을 보강
		CameraCurrentFOV = FMath::FInterpTo(
			CameraCurrentFOV,
			CameraDefaultFOV,
			DeltaTime,
			ZoomInterpSpeed);

	}
	Camera->SetFieldOfView(CameraCurrentFOV);
}

void AMyCharacter::SetLookRates()
{
	if (bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AMyCharacter::CalculateCrosshairSpread(float DeltaTime)
{
	FVector2D WalkSpeddRange(0.f, 600.f);
	FVector2D VelocityMultiplierRange(0.f, 1.f);
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	
	//[Input:Range] inclusive로 고정된 주어진 값에 대해 [Output:Range] Inclusive에서 해당 백분율을 반환합니다.
	CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped
	(WalkSpeddRange, VelocityMultiplierRange,Velocity.Size());

	// 공중에 있을떄 크로스 헤어
	if (GetCharacterMovement()->IsFalling()) //공중에 있나
	{
		//천천히 크로스헤어가 퍼짐
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor, 1, DeltaTime, 2.25f);
	}
	else //바닥에 닿았으면
	{
		//크로스헤어를 빠르게 좁힘
		CrosshairInAirFactor = FMath::FInterpTo(
			CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
	}

	//크로스헤어 조준중일때 계산
	if (bAiming) //조준중인가
	{
		//십자선 축소
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor, 0.6f,
			DeltaTime,
			30.f);
	}
	else // 조준중이 아니면
	{
		//십자선 복구
		CrosshairAimFactor = FMath::FInterpTo(
			CrosshairAimFactor, 0.f,
			DeltaTime,
			30.f);
	}

	//0.05초뒤에 참
	if (bFiringBullet)
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,
			0.3f, DeltaTime, 60.f);
	}
	else
	{
		CrosshairShootingFactor = FMath::FInterpTo(CrosshairShootingFactor,
			0.f, DeltaTime, 60.f);
	}

	CrosshairSpreadMultiplire = 
		0.5f +
		CrosshairVelocityFactor +
		CrosshairInAirFactor -
		CrosshairAimFactor +
		CrosshairShootingFactor;
}

void AMyCharacter::StartCrosshairBulletFire()
{
	bFiringBullet = true;

	//0.05초뒤에 FinishCrosshairBulletFire함수를 불러옴
	GetWorldTimerManager().SetTimer(
		CrosshairShootTimer,
		this, &AMyCharacter::FinishCrosshairBulletFire, 
		ShootTimeDuration);
}

void AMyCharacter::FinishCrosshairBulletFire()
{
	bFiringBullet = false;
	GetWorldTimerManager().ClearTimer(CrosshairShootTimer);
}

void AMyCharacter::FireButtonPressed()
{
	bFireButtonPressed = true;
	Fire();
	
}

void AMyCharacter::FireButtonReleased()
{
	bFireButtonPressed = false;
}

void AMyCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer,
		this,
		&AMyCharacter::AutoFireReset,
		AutoAmticFireRate);
	
}

void AMyCharacter::AutoFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed)
		{
			Fire();
		}
	}
	else
	{
		//무기 재장전
		ReloadWeapon();
	}
	
}

bool AMyCharacter::TraceUnderCrosshairs(FHitResult& OutHitResult, FVector& OutHitLocation)
{
	// 뷰포트의 현재 크기를 가져온다.
	FVector2D ViewPortSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewPortSize);
	}

	// 십자선 위치 설정
	FVector2D CrosshairLocation(ViewPortSize.X / 2.f, ViewPortSize.Y / 2.f);
	CrosshairLocation.Y -= 50.f;
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	//십자선 방향과 위치 파악
	//화면 공간 위치를 나타내는 FVector2D를 세계 공간 위치를 나타내는 FVector로 변환
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection);

	if (bScreenToWorld)
	{
		//
		const FVector Start = CrosshairWorldPosition;
		const FVector End = Start+ CrosshairWorldDirection * 50'000.f;
		OutHitLocation = End;

		//크로스헤어로 트레이스 추적
		GetWorld()->LineTraceSingleByChannel(OutHitResult,
			Start, End,
			ECollisionChannel::ECC_Visibility);
		if (OutHitResult.bBlockingHit)
		{
			OutHitLocation = OutHitResult.Location;
			return true;
		}
	}

	return false;
}

void AMyCharacter::TraceForItems()
{
	if(bShouldTraceForItems)
	{
		FHitResult ItemTraceResult;
		FVector hitLocation;
		TraceUnderCrosshairs(ItemTraceResult, hitLocation);
		if (ItemTraceResult.bBlockingHit)
		{
			TraceHitItem = Cast<AItem>(ItemTraceResult.GetActor());
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
			}
			// 마지막에 겹친 아이템이 있는지
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//히트 아이템이 널이고 마지막 아이템이 다르면 위젯을 꺼줌
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				}
			}
			// 히트 아이템을 참조
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);

	}

}

AWeapon* AMyCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		return GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
	}
	return nullptr;
}

void AMyCharacter::EquipWeapon(AWeapon* WeaponToEquip)
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
		//장착한 무기는 스폰된 무기
		EquipeedWeapon = WeaponToEquip;
		//장착한 상태
		EquipeedWeapon->SetItemState(EItemState::EIS_Equipped);
	
	}
}

void AMyCharacter::DropWeapon()
{
	if (EquipeedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquipeedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
	
		EquipeedWeapon->SetItemState(EItemState::EIS_Falling);
		EquipeedWeapon->ThrowWeapon();
	}
}

void AMyCharacter::SwapWeapon(AWeapon* WeaponToSwap)
{
	DropWeapon();
	EquipWeapon(WeaponToSwap);
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
}
void AMyCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9MMAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}
bool AMyCharacter::WeaponHasAmmo()
{
	if (EquipeedWeapon == nullptr) return false;

	return EquipeedWeapon->GetAmmo() > 0;
}
void AMyCharacter::PlayFireSound()
{
	//사운드 플레이
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
}
void AMyCharacter::SendBullet()
{
	//총알 보내기
	const USkeletalMeshSocket* Weapon_Socket =
		EquipeedWeapon->GetItemMesh()->GetSocketByName("Weapon_Socket");
	if (Weapon_Socket)
	{
		const FTransform SocketTransform = Weapon_Socket->GetSocketTransform(EquipeedWeapon->GetItemMesh());

		if (MuzzleFalsh)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFalsh, SocketTransform);
		}

		FVector BeamEnd;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamEnd);
		if (bBeamEnd)
		{
			if (ImpactParticle) //히트지점 파티클
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					ImpactParticle, BeamEnd);
			}

			if (BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), BeamParticle, SocketTransform);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamEnd);//어디까지 연기가 가게 할건지
				}
			}
		}
	}
}
void AMyCharacter::PlayGunFireMontage()
{
	// 애니메이션 플레이
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && FireMontage)
	{
		AnimInstance->Montage_Play(FireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}
bool AMyCharacter::CaaryinhAmmo()
{
	if (EquipeedWeapon == nullptr) return false;

	auto AmmoType = EquipeedWeapon->GetAmmoType();

	if (AmmoMap.Contains(AmmoType)) //포함 여부를 확인
	{
		return AmmoMap[AmmoType] > 0;
	}
	return false;
}
FVector AMyCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation = Camera->GetComponentLocation();
	const FVector CameraForward = Camera->GetForwardVector();

	//CameraWorldLocation + Forward *A + Up *B
	return CameraWorldLocation + CameraForward * CameraInterpDistance
		+ FVector(0.f, 0.f, CameraInterpElevation);
	

}

void AMyCharacter::GetPickupItem(AItem* Item)
{
	if (Item->GetEquipSound())
	{
		UGameplayStatics::PlaySound2D(this, Item->GetEquipSound());
	}
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		SwapWeapon(Weapon);
	}
}
