// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Enemy.h"
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include "Sound/SoundBase.h"
#include <Blueprint/UserWidget.h>
#include "Kismet/KismetMathLibrary.h"
#include <DrawDebugHelpers.h>
#include <../Public/AIController/MyAIController.h>
#include "../public/Character/MyCharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AgrosSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AgroSphere"));
	AgrosSphere->SetupAttachment(RootComponent);

	CombatRangeSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CombatRangeSphere"));
	CombatRangeSphere->SetupAttachment(RootComponent);

	LeftWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Left Weapon Box"));
	LeftWeaponCollision->SetupAttachment(GetMesh(), FName("weaponL_Socket"));

	RightWeaponCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Right Weapon Collision"));
	RightWeaponCollision->SetupAttachment(GetMesh(), FName("weaponR_Socket"));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	AgrosSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::AgroSphereOverlap);
	
	//스페어 겹치면 공격
	CombatRangeSphere->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::CombatRangeOverlap);
	CombatRangeSphere->OnComponentEndOverlap.AddDynamic(this, &AEnemy::CombatRangeEndOverlap);

	//무기 겹치면 데미지
	LeftWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnLeftWeaponOverlap);
	RightWeaponCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemy::OnRightWeaponOverlap);
	
	//공격전에 데미지 주면 안됨
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightWeaponCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightWeaponCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightWeaponCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	//AI 컨트롤러 가져오기
	EnemyController = Cast<AMyAIController>(GetController());

	const FVector WorldPatrolPoint = UKismetMathLibrary::TransformLocation(
		GetActorTransform(),PatrolPoint);
	const FVector WorldPatrolPoint2 = UKismetMathLibrary::TransformLocation(
		GetActorTransform(), PatrolPoint2);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint,
		25.f,
		12,
		FColor::Red,
		true
	);

	DrawDebugSphere(
		GetWorld(),
		WorldPatrolPoint2,
		25.f,
		12,
		FColor::Red,
		true
	);
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint"),
			WorldPatrolPoint);

		EnemyController->GetBlackboardComponent()->SetValueAsVector(
			TEXT("PatrolPoint2"),
			WorldPatrolPoint2);

		EnemyController->RunBehaviorTree(BeHaviorTree);
	}


}

void AEnemy::ShowHealthBar_Implementation()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	GetWorldTimerManager().SetTimer(
		HealthBarTimer,
		this,
		&AEnemy::HideHealthBar,
		HealthBarDisplayTime);
}

void AEnemy::PlayHitMontage(FName Section, float PlayRate)
{
	if (bCanHitReact)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && HitMontage)
		{
			AnimInstance->Montage_Play(HitMontage, PlayRate);
			AnimInstance->Montage_JumpToSection(Section, HitMontage);
		}

		
		bCanHitReact = false;
		const float HitReactTime = FMath::FRandRange(HitReactTimeMin, HitRactTimeMax);
		GetWorldTimerManager().SetTimer(
			HitReactTimer, 
			this,
			&AEnemy::ResetHitReactTimer, 
			HitReactTime);
	}

}

void AEnemy::PlayAttackMontage(FName Section, float PlayRate)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, PlayRate);
		AnimInstance->Montage_JumpToSection(Section, AttackMontage);
	}
}

void AEnemy::ResetHitReactTimer()
{
	bCanHitReact = true;
}

void AEnemy::StoreHitNumber(UUserWidget* HitNumber, FVector Location)
{
	HitNumbers.Add(HitNumber, Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this, FName("DestoryNumber"), HitNumber);

	GetWorldTimerManager().SetTimer(
		HitNumberTimer,
		HitNumberDelegate,
		HitNumberDestroyTime,
		false);
		
}

void AEnemy::DestoryNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemy::UpdateHitNumbers()
{
	for (auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber = HitPair.Key;
		const FVector Location = HitPair.Value;
		FVector2D ScrePosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScrePosition);
		HitNumber->SetPositionInViewport(ScrePosition);
	}
}

void AEnemy::AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto Character = Cast<AMyCharacter>(OtherActor);

	//겹친 캐릭터가 있으면
	if (Character)
	{
		//블랙보드 타켓을 부딫힌 캐릭터로 바꿔줌
		EnemyController->GetBlackboardComponent()->SetValueAsObject(
			TEXT("Target"),
			Character);
	}
}

void AEnemy::AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}

void AEnemy::SetStunned(bool Stunned)
{
	bStunned = Stunned;
	if (EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), bStunned);
	}
}

void AEnemy::CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AMyCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		bInAttackRange = true;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), true);
		}
	}

}

void AEnemy::CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor == nullptr) return;
	auto ShooterCharacter = Cast<AMyCharacter>(OtherActor);
	if (ShooterCharacter)
	{
		bInAttackRange = false;
		if (EnemyController)
		{
			EnemyController->GetBlackboardComponent()->SetValueAsBool(TEXT("InAttackRange"), false);
		}
	}
	
}

FName AEnemy::GetAttackSectionName()
{
	FName SectionName;
	const int32 Section = FMath::RandRange(1, 4);
	switch (Section)
	{
	case 1:
		SectionName = AttackLFast;
		break;
	case 2:
		SectionName = AttackRFast;
		break;
	case 3:
		SectionName = AttackL;
		break;
	case 4:
		SectionName = AttackR;
		break;
	}
	return SectionName;
}

void AEnemy::OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AMyCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character, LeftWeaponSocket);
	}

}

void AEnemy::OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto Character = Cast<AMyCharacter>(OtherActor);
	if (Character)
	{
		DoDamage(Character);
		SpawnBlood(Character, RightWeaponSocket);
	}
}

void AEnemy::ActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateLeftWeapon()
{
	LeftWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::ActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AEnemy::DeActivateRightWeapon()
{
	RightWeaponCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEnemy::DoDamage(AMyCharacter* Victim)
{
	if (Victim == nullptr) return;

	UGameplayStatics::ApplyDamage(
		Victim,
		BaseDamage,
		EnemyController,
		this,
		UDamageType::StaticClass()
	);
		if (Victim->GetMeleeImpactSound())
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				Victim->GetMeleeImpactSound(),
				GetActorLocation()
			);
		}

}

void AEnemy::SpawnBlood(AMyCharacter* Victime, FName SocketName)
{
	const USkeletalMeshSocket* TipSocket = GetMesh()->GetSocketByName(SocketName);
	if (TipSocket)
	{
		const FTransform SocketTansform = TipSocket->GetSocketTransform(GetMesh());
		if (Victime->GetBloodParticle())
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				Victime->GetBloodParticle(),
				SocketTansform);
		}
	}

}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdateHitNumbers();

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
void AEnemy::BulletHit_Implementation(FHitResult HitResult)
{
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles,
			HitResult.Location, FRotator(0.f), true);
	}
	ShowHealthBar();

	const float Stunned = FMath::FRandRange(0.1f, 1.f);
	if (Stunned <= StunChance)
	{
		//
		PlayHitMontage(FName("Front"));
		SetStunned(true);
	}

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Health - DamageAmount <= 0.f)
	{
		Health = 0.f;
	}
	else
	{
		Health -= DamageAmount;
	}

	return DamageAmount;
}
