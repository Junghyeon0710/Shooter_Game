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
#include "Components/CapsuleComponent.h"
#include "Item/Ammo.h"
#include <PhysicalMaterials/PhysicalMaterial.h>
#include "../Shooter_Game.h"
#include "../Public/Interface/BulletHitInterface.h"
#include "../Public/Character/Enemy.h"
#include "AIController/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	//보간 컴포넌트
	WeaponInterpComp = CreateDefaultSubobject<USceneComponent>(TEXT("Weapon Interpolation Componet"));
	WeaponInterpComp->SetupAttachment(Camera);

	InterpComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 1"));
	InterpComp1->SetupAttachment(Camera);

	InterpComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 2"));
	InterpComp2->SetupAttachment(Camera);

	InterpComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 3"));
	InterpComp3->SetupAttachment(Camera);

	InterpComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 4"));
	InterpComp4->SetupAttachment(Camera);

	InterpComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 5"));
	InterpComp5->SetupAttachment(Camera);

	InterpComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("Interpolation Componet 6"));
	InterpComp6->SetupAttachment(Camera);

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
	Inventory.Add(EquipeedWeapon);
	EquipeedWeapon->SetSlotIndex(0);
	//무기 색깔 없음
	EquipeedWeapon->DisableCustomDepth();
	EquipeedWeapon->DisableGlowMaterial();
	EquipeedWeapon->SetCharacter(this);
	InitializeAmmoMap();

	//보간 구조체 배열에 저장 및 초기화
	InitializeInterpLocation();	
	
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
	
	InterpCapsuleHalfHeight(DeltaTime);
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AMyCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Started, this, &AMyCharacter::AimingButtonPressed);
		EnhancedInputComponent->BindAction(AimingAction, ETriggerEvent::Completed, this, &AMyCharacter::AimingButtonReleased);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AMyCharacter::SelectButtonPressed);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AMyCharacter::SelectButtonReleased);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AMyCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AMyCharacter::CrouchingPressed);
		EnhancedInputComponent->BindAction(FKeyhAction, ETriggerEvent::Started, this, &AMyCharacter::FKeyPressed);
		EnhancedInputComponent->BindAction(Key1Action, ETriggerEvent::Started, this, &AMyCharacter::Key1Pressed);
		EnhancedInputComponent->BindAction(Key2Action, ETriggerEvent::Started, this, &AMyCharacter::Key2Pressed);
		EnhancedInputComponent->BindAction(Key3Action, ETriggerEvent::Started, this, &AMyCharacter::Key3Pressed);
		EnhancedInputComponent->BindAction(Key4Action, ETriggerEvent::Started, this, &AMyCharacter::Key4Pressed);
		EnhancedInputComponent->BindAction(Key5Action, ETriggerEvent::Started, this, &AMyCharacter::Key5Pressed);


	}
}

float AMyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
		Die();

		auto EnemyController = Cast<AMyAIController>(EventInstigator);
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(
				FName("CharacterDead"), true);
		}
	}
	else
	{
		Health -= DamageAmount;
		
	}

	return DamageAmount;
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

void AMyCharacter::ResetPicukSoundTimer()
{
	bShouldPlayPickupSound = true;
}

void AMyCharacter::ResetEquipSoundTimer()
{
	bShouldPlayEquipSound = true;
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

		if (EquipeedWeapon->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			//슬라이 타이머 시작
			EquipeedWeapon->StartSlideTimer();
		}
	}

}

void AMyCharacter::Jump()
{
	if (bCrouching)
	{
		bCrouching = false;
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
	else
	{
		ACharacter::Jump();
	}
}

bool AMyCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FHitResult& OutHitResult)
{
	FVector OutBeamLocation;
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

	const FVector WeaponTraceStart = MuzzleSocketLocation;
	const FVector StartToEnd = OutBeamLocation - MuzzleSocketLocation; //시작부터 끝에 거리는 끝에서 처음 거리를 뺴면된다.
	const FVector WeaponTraceEnd = MuzzleSocketLocation + StartToEnd *1.25;
	GetWorld()->LineTraceSingleByChannel(
		OutHitResult,
		WeaponTraceStart,
		WeaponTraceEnd,
		ECollisionChannel::ECC_Visibility
	);

	if (!OutHitResult.bBlockingHit) //// 총입구에서 발사지점 사이에 물체가 있다면 
	{
		OutHitResult.Location = OutBeamLocation;
		return false;
	}
	return true;
	
}

void AMyCharacter::AimingButtonPressed()
{

	bAimingButtonPressed = true;
	if (CombatState != ECombatState::ECS_Reloading &&
		CombatState != ECombatState::ECS_Equipping &&
		CombatState != ECombatState::ECS_Stuneed) Aim();

}

void AMyCharacter::AimingButtonReleased()
{
	bAimingButtonPressed = false;
	StopAiming();
}

void AMyCharacter::SelectButtonPressed()
{
	if (CombatState != ECombatState::ECS_Unoccupied) return;
	if (TraceHitItem)
	{
		TraceHitItem->StartItemCurve(this,true);
		TraceHitItem = nullptr;
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

	// 맞는 탄약이 있는지 체크 && 탄약이 안 꽉찼는지
	if (CaaryinhAmmo() && !EquipeedWeapon->ClipIsFull())
	{
		if (bAiming) StopAiming();
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
	if (CombatState == ECombatState::ECS_Stuneed) return;

	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed) Aim();

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

void AMyCharacter::FinishEquipping()
{
	if (CombatState == ECombatState::ECS_Stuneed) return;

	CombatState = ECombatState::ECS_Unoccupied;
	if (bAimingButtonPressed)
	{
		Aim();
	}
}

void AMyCharacter::CrouchingPressed()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bCrouching = !bCrouching;
	}
	if (bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
		GetCharacterMovement()->GroundFriction = CrouchGroundFriction;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
		GetCharacterMovement()->GroundFriction = BaseGroundFriction;
	}
}

void AMyCharacter::FKeyPressed()
{
	if (EquipeedWeapon->GetSlotIndex() == 0) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 0);
}

void AMyCharacter::Key1Pressed()
{
	
	if (EquipeedWeapon->GetSlotIndex() == 1) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 1);
	
}

void AMyCharacter::Key2Pressed()
{
	if (EquipeedWeapon->GetSlotIndex() == 2) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 2);
}

void AMyCharacter::Key3Pressed()
{
	if (EquipeedWeapon->GetSlotIndex() == 3) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 3);
}

void AMyCharacter::Key4Pressed()
{
	if (EquipeedWeapon->GetSlotIndex() == 4) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 4);
}

void AMyCharacter::Key5Pressed()
{
	if (EquipeedWeapon->GetSlotIndex() == 5) return;

	ExchangeInventoryIrems(EquipeedWeapon->GetSlotIndex(), 5);
}

void AMyCharacter::ExchangeInventoryIrems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	const bool bCanExchangeItems =
		(CurrentItemIndex != NewItemIndex) &&
		(NewItemIndex < Inventory.Num()) &&
		(CombatState == ECombatState::ECS_Unoccupied || CombatState == ECombatState::ECS_Equipping);

	if (bCanExchangeItems)
	{
		if (bAiming)
		{
			StopAiming();
		}
		auto OldEquippedWeapon = EquipeedWeapon;
		auto NewWeapon = Cast<AWeapon>(Inventory[NewItemIndex]);

		EquipWeapon(NewWeapon);
		OldEquippedWeapon->SetItemState(EItemState::EIS_PickedUp);
		NewWeapon->SetItemState(EItemState::EIS_Equipped);

		CombatState = ECombatState::ECS_Equipping;
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && EquipMontage)
		{
			AnimInstance->Montage_Play(EquipMontage, 1.0);
			AnimInstance->Montage_JumpToSection(FName("Equip"));
		}
		NewWeapon->PlayEquipSound(true);
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
	if (EquipeedWeapon == nullptr) return;
	CombatState = ECombatState::ECS_FireTimerInProgress;

	GetWorldTimerManager().SetTimer(AutoFireTimer,
		this,
		&AMyCharacter::AutoFireReset,
		EquipeedWeapon->GetAutoFireRate());
	
}

void AMyCharacter::AutoFireReset()
{
	if (CombatState == ECombatState::ECS_Stuneed) return;
	CombatState = ECombatState::ECS_Unoccupied;
	if (EquipeedWeapon == nullptr) return;
	if (WeaponHasAmmo())
	{
		if (bFireButtonPressed&& EquipeedWeapon->GetAutomatic())
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
	//CrosshairLocation.Y -= 50.f;
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
			const auto TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			if (TraceHitWeapon)
			{
				if (HighlightedSlot == -1)
				{
					//현재 강조된 슬롯이 없다.
					HighlightInventorySlot();
				}
			}
			else
			{
				if (HighlightedSlot != -1)
				{
					UnHighlightInventorySlot();
				}
			}
			if (TraceHitItem && TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
			{
				TraceHitItem = nullptr;
			}
			if (TraceHitItem && TraceHitItem->GetPickupWidget())
			{
				TraceHitItem->GetPickupWidget()->SetVisibility(true);
				TraceHitItem->EnableCustomDepth();

				if (Inventory.Num() >= INVENTORY_CAPACITY)
				{
					//인벤토리 꽉참
					TraceHitItem->SetCharacterInventoryFull(true);
				}
				else //인벤토리 안꽉참
				{
					TraceHitItem->SetCharacterInventoryFull(false);
				}
			}
			// 마지막에 겹친 아이템이 있는지
			if (TraceHitItemLastFrame)
			{
				if (TraceHitItem != TraceHitItemLastFrame)
				{
					//히트 아이템이 널이고 마지막 아이템이 다르면 위젯을 꺼줌
					TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
				
					TraceHitItemLastFrame->DisableCustomDepth();
				}
			}
			// 히트 아이템을 참조
			TraceHitItemLastFrame = TraceHitItem;
		}
	}
	else if (TraceHitItemLastFrame)
	{
		TraceHitItemLastFrame->GetPickupWidget()->SetVisibility(false);
		TraceHitItemLastFrame->DisableCustomDepth();

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

void AMyCharacter::EquipWeapon(AWeapon* WeaponToEquip , bool bSwapping)
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
			//장착돤 아이넴이 없다.
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}
		else if(!bSwapping)
		{
			EquipItemDelegate.Broadcast(EquipeedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
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

	if (Inventory.Num() - 1 >= EquipeedWeapon->GetSlotIndex())
	{
		Inventory[EquipeedWeapon->GetSlotIndex()] = WeaponToSwap;
		WeaponToSwap->SetSlotIndex(EquipeedWeapon->GetSlotIndex());
	}

	DropWeapon();
	EquipWeapon(WeaponToSwap,true);
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
	if (EquipeedWeapon == nullptr) return;
	//사운드 플레이
	if (EquipeedWeapon->GetFireSound())
	{
		UGameplayStatics::PlaySound2D(this,EquipeedWeapon->GetFireSound());
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

		if (EquipeedWeapon->GetMuzzleFlash())
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), EquipeedWeapon->GetMuzzleFlash(), SocketTransform);
		}

		FHitResult BeamHitReuslt;
		bool bBeamEnd = GetBeamEndLocation(SocketTransform.GetLocation(), BeamHitReuslt);
		if (bBeamEnd)
		{
			//인터페이스가 존재하나요
			if (BeamHitReuslt.GetActor())
			{
				IBulletHitInterface* BulletHitInterface = Cast<IBulletHitInterface>(BeamHitReuslt.GetActor());
				if (BulletHitInterface)
				{
					BulletHitInterface->BulletHit_Implementation(BeamHitReuslt,this,GetController());
				}

				AEnemy* HitEnmey = Cast<AEnemy>(BeamHitReuslt.GetActor());
				if (HitEnmey)
				{
					int32 Damage;
					//헤드샷
					if (BeamHitReuslt.BoneName.ToString() == HitEnmey->GetHeadBone())
					{
						Damage = EquipeedWeapon->GetHeadShotDamage();
						UGameplayStatics::ApplyDamage(
							BeamHitReuslt.GetActor(),
							EquipeedWeapon->GetHeadShotDamage(),
							GetController(),
							this,
							UDamageType::StaticClass());
						HitEnmey->ShowHitNumber(Damage, BeamHitReuslt.Location,true);
					}
					else
					{
						Damage = EquipeedWeapon->GetDamage();
						UGameplayStatics::ApplyDamage(
							BeamHitReuslt.GetActor(),
							EquipeedWeapon->GetDamage(),
							GetController(),
							this,
							UDamageType::StaticClass());
						HitEnmey->ShowHitNumber(Damage, BeamHitReuslt.Location,false);
					}
					
				}
			}
			else
			{
				if(ImpactParticle) //히트지점 파티클
			
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),
					ImpactParticle, BeamHitReuslt.Location);
			}
		
			if (BeamParticle)
			{
				UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(
					GetWorld(), BeamParticle, SocketTransform);
				if (Beam)
				{
					Beam->SetVectorParameter(FName("Target"), BeamHitReuslt.Location);//어디까지 연기가 가게 할건지
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
void AMyCharacter::InterpCapsuleHalfHeight(float DeltaTime)
{
	float TargetCapsuleHealfHeight;

	if (bCrouching)
	{
		TargetCapsuleHealfHeight = CrouchCapsulHalfHeight;
	}
	else
	{
		TargetCapsuleHealfHeight = StandingCapsuleHalfHeight;
	}
	const float InterpHalfHeight = FMath::FInterpTo(
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		TargetCapsuleHealfHeight,
		DeltaTime, 20.f);
	
	// 앉으면 마이너스 값 Mesh를 올려야함
	const float DeltaCapsuleHalfHeight = InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	const FVector MeshOffset(0.f, 0.f, -DeltaCapsuleHalfHeight);
	GetMesh()->AddLocalOffset(MeshOffset);
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}
void AMyCharacter::Aim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchMovementSpeed;
}
void AMyCharacter::StopAiming()
{
	bAiming = false;
	if (!bCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	}
}
void AMyCharacter::PickupAmmo(AAmmo* Ammo)
{
	if (AmmoMap.Find(Ammo->GetAmmoType()))
	{
		//AmmoMap 타입에 탄약 갯수
		int32 AmmoCount = AmmoMap[Ammo->GetAmmoType()];
		AmmoCount += Ammo->GetItemCount();
		//총알 갯수를 바꿔줌
		AmmoMap[Ammo->GetAmmoType()] = AmmoCount;
	}

	if (EquipeedWeapon->GetAmmoType() == Ammo->GetAmmoType())
	{
		//총알이 하나도 없으면 재장전
		if (EquipeedWeapon->GetAmmo() == 0)
		{
			ReloadWeapon();
		}
	}
	TraceHitItem = nullptr;
	TraceHitItemLastFrame = nullptr;
	Ammo->Destroy();
}
void AMyCharacter::InitializeInterpLocation()
{
	FInterpLocation WeaponLocation = { WeaponInterpComp, 0 };
	InterpLocation.Add(WeaponLocation);

	FInterpLocation InterpLoc1 = { InterpComp1,0 };
	InterpLocation.Add(InterpLoc1);

	FInterpLocation InterpLoc2 = { InterpComp2,0 };
	InterpLocation.Add(InterpLoc2);

	FInterpLocation InterpLoc3 = { InterpComp3,0 };
	InterpLocation.Add(InterpLoc3);

	FInterpLocation InterpLoc4 = { InterpComp4,0 };
	InterpLocation.Add(InterpLoc4);

	FInterpLocation InterpLoc5 = { InterpComp5,0 };
	InterpLocation.Add(InterpLoc5);

	FInterpLocation InterpLoc6 = { InterpComp6,0 };
	InterpLocation.Add(InterpLoc5);
}
int32 AMyCharacter::GetEmptyInventorySlot()
{
	for (int32 i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	if (Inventory.Num() < INVENTORY_CAPACITY)
	{
		return Inventory.Num();
	}
	return -1; // 인벤토리 꽉참
}
void AMyCharacter::HighlightInventorySlot()
{
	const int32 EmptySlot = GetEmptyInventorySlot();
	HighlightIconDelegate.Broadcast(EmptySlot, true);
	HighlightedSlot = EmptySlot;
}
EPhysicalSurface AMyCharacter::GetSurfaceType()
{
	
	FHitResult HitResult;
	const FVector Start = GetActorLocation();
	const FVector End = Start + FVector(0.f, 0.f, -400.f);
	FCollisionQueryParams QueryParms;
	QueryParms.bReturnPhysicalMaterial = true;

	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start, End,
		ECollisionChannel::ECC_Visibility,
		QueryParms);
	
	return UPhysicalMaterial::DetermineSurfaceType(HitResult.PhysMaterial.Get());
}
void AMyCharacter::EndStun()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bAimingButtonPressed)
	{
		Aim();
	}
}
void AMyCharacter::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
	}
}
void AMyCharacter::FinishDeath()
{
	GetMesh()->bPauseAnims = true;
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		DisableInput(PC);
	}
	
}
void AMyCharacter::UnHighlightInventorySlot()
{
	HighlightIconDelegate.Broadcast(HighlightedSlot, false);
	HighlightedSlot = -1;
}
void AMyCharacter::Stun()
{
	if (Health <= 0.f) return;
	CombatState = ECombatState::ECS_Stuneed;
	StopAiming();
	UAnimInstance* Instance = GetMesh()->GetAnimInstance();
	if (Instance && HitReactMontage)
	{
		Instance->Montage_Play(HitReactMontage);
		//Instance->Montage_JumpToSection("Front", HitReactMontage);
	}
}
int32 AMyCharacter::GetInterpLocationIndex()
{
	int32 LowstIndex = 1;
	int32 LowsetCount = INT_MAX;
	for (int32 i = 1; i < InterpLocation.Num(); i++)
	{
		if (InterpLocation[i].ItemCount < LowsetCount)
		{
			LowstIndex = i;
			LowsetCount = InterpLocation[i].ItemCount;
		}
	}
	return LowstIndex;
}
void AMyCharacter::IncrementInterpLocItemCount(int32 Index, int32 Amount)
{
	if (Amount < -1 || Amount>1) return;

	if (InterpLocation.Num() >= Index)
	{
		InterpLocation[Index].ItemCount += Amount;
	}
}
void AMyCharacter::StartPickupSoundTimer()
{
	bShouldPlayPickupSound = false;
	GetWorldTimerManager().SetTimer(
		PickupSoundTimer, 
		this,
		&AMyCharacter::ResetPicukSoundTimer,
		PicupSoundResetTime);
}
void AMyCharacter::StartEquipSoundTimer()
{
	bShouldPlayEquipSound = false;
	GetWorldTimerManager().SetTimer(
		EqiupSoundTimer,
		this,
		&AMyCharacter::ResetEquipSoundTimer,
		EquipSOundResetTime);
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
	
	Item->PlayEquipSound();
	
	auto Weapon = Cast<AWeapon>(Item);
	if (Weapon)
	{
		if (Inventory.Num() < INVENTORY_CAPACITY)
		{
			Weapon->SetSlotIndex(Inventory.Num());
			Inventory.Add(Weapon);
			Weapon->SetItemState(EItemState::EIS_PickedUp);
		}
		else //인벤토리가 꽉 참
		{
			SwapWeapon(Weapon);
		}
		
	}

	auto Ammo = Cast<AAmmo>(Item);
	if (Ammo)
	{
		PickupAmmo(Ammo);
	}
}

FInterpLocation AMyCharacter::GetInterpLocation(int32 Index)
{
	if (Index <= InterpLocation.Num())
	{
		return InterpLocation[Index];
	}
	return FInterpLocation();
}
