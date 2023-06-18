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

// Sets default values
AMyCharacter::AMyCharacter() :
	BaseTurnRate(45.f),
	BaseLookUpRate(45.f)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true; // 폰에 따라 회전
	SpringArm->SocketOffset = FVector(0.5f, 50.f, 50.f);

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
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {

		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::Fire);
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

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X*BaseTurnRate*GetWorld()->GetDeltaSeconds());
		AddControllerPitchInput(LookAxisVector.Y*BaseLookUpRate* GetWorld()->GetDeltaSeconds());
	}
}

void AMyCharacter::Fire()
{
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	}
	const USkeletalMeshSocket* Weapon_Socket = GetMesh()->GetSocketByName("Weapon_Socket");
	if (Weapon_Socket)
	{
		const FTransform SocketTransform = Weapon_Socket->GetSocketTransform(GetMesh());

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

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && FireMontage)
		{
			AnimInstance->Montage_Play(FireMontage);
			AnimInstance->Montage_JumpToSection(FName("StartFire"));
		}
	}
}

bool AMyCharacter::GetBeamEndLocation(const FVector& MuzzleSocketLocation, FVector& OutBeamLocation)
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

	//deprojection을 성공했나
	if (bScreenToWorld)
	{
		FHitResult ScreenTraceHit;
		const FVector Start = CrosshairWorldPosition;
		//const FQuat Rotation = SocketTransform.GetRotation();
		//const FVector RotationAxix = Rotation.GetAxisX(); //x축 구해준다
		const FVector End = CrosshairWorldPosition + CrosshairWorldDirection * 50'000.f;

		// 안개 지점을 라인트레이서 엔드포인터로 저장
		OutBeamLocation = End;

		//크로스헤어로 트레이스 추적
		GetWorld()->LineTraceSingleByChannel(ScreenTraceHit,
			Start, End,
			ECollisionChannel::ECC_Visibility);

		if (ScreenTraceHit.bBlockingHit) // 맞은게 있다면
		{
			//DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 5.f);
			//DrawDebugPoint(GetWorld(), ScreenTraceHit.Location, 5.f, FColor::Red, false, 5.f);

			//beam 끔점은 현재 맞은 위치
			OutBeamLocation = ScreenTraceHit.Location;

		}

		//두번째 라인트레이스 총위치에서			
		FHitResult WeaponTraceHit;
		const FVector WeaponTraceStart = MuzzleSocketLocation;
		const FVector WeaponTraceEnd = OutBeamLocation;
		GetWorld()->LineTraceSingleByChannel(
			WeaponTraceHit,
			WeaponTraceStart,
			WeaponTraceEnd,
			ECollisionChannel::ECC_Visibility
		);

		if (WeaponTraceHit.bBlockingHit) //// 총입구에서 발사지점 사이에 물체가 있다면 
		{
			OutBeamLocation = WeaponTraceHit.Location;
		}
		return true;
	}
	return false;
}
