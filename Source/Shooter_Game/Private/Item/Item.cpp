// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"
#include "Character/MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ItemMesh"));
	RootComponent = ItemMesh;

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetupAttachment(ItemMesh);
	CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

	PickuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickukWidget"));
	PickuWidget->SetupAttachment(RootComponent);

	AreaSphere = CreateAbstractDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickuWidget)
	{
		PickuWidget->SetVisibility(false);
	}
	
	//항목의 휘귀도의 따라 별 설정
	SetActiveStars();

	AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	AreaSphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);

	//아이템 속성 설정
	SetItemProperties(ItemState);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		AMyCharacter* ShooterCharaceter = Cast<AMyCharacter>(OtherActor);
		if (ShooterCharaceter)
		{
			ShooterCharaceter->IncrementOverlappedItemCount(1);
	
		}
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		AMyCharacter* ShooterCharaceter = Cast<AMyCharacter>(OtherActor);
		if (ShooterCharaceter)
		{
			ShooterCharaceter->IncrementOverlappedItemCount(-1);
		}
	}
}

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

void AItem::SetItemProperties(EItemState State)
{
	switch (State)
	{
	case EItemState::EIS_Pickup:
		// 아이템 속성 변경
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		

		// 스피어 속성 설정
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		

		//박스 콜리전 설정
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
		

		break;
	case EItemState::EIS_EquipInterping:
		PickuWidget->SetVisibility(false);
		// 아이템 속성 변경
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		// 스피어 속성 설정
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		//박스 콜리전 설정
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_Equipped:
		PickuWidget->SetVisibility(false);
		// 아이템 속성 변경
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	

		// 스피어 속성 설정
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		

		//박스 콜리전 설정
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		

		break;
	case EItemState::EIS_Falling:
		// 아이템 속성 변경
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic,ECollisionResponse::ECR_Block);
		ItemMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);
		
		// 스피어 속성 설정
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		

		//박스 콜리전 설정
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		

		break;
	case EItemState::EIS_Max:
		break;
	default:
		break;
	}
}

void AItem::FinishInterping()
{
	bInterping = false;
	if (Character)
	{
		//구조체 아이템 카운트를 다시빼줌
		Character->IncrementInterpLocItemCount(InterpLocIndex, -1);
		Character->GetPickupItem(this);
	}
	//스케일 정상적으로 설정
	SetActorScale3D(FVector(1.f));
	
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//아이템 보간
	ItemInterp(DeltaTime);
}

void AItem::ItemInterp(float DeltaTime)
{
	if (!bInterping) return;

	if (Character && ItemZCurve)
	{
		//타이머 경과시관을 알려줌
		const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
		//경과시간에 따른 커브 값 반환
		const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime);

		//아이템 초기 위치 가져오기
		FVector ItemLocation = ItemInterpStartLocation;

		//카메라 앞 위치 가쳐오기
		const FVector CameraInterpLocation = GetInterpLocation();
		
		// 아이템에서 카메라까지 보간 위치는 백터 X , Y는 0 카메라 Z위치값이랑 아이템 Z위치 값을 빼줌
		//그러면 아이템에서 카메라 사이에 Z값을 수할 수 있음
		const FVector ItemToCamera = FVector(0.f, 0.f, (CameraInterpLocation - ItemLocation).Z);
		//커버값 스케일 곱하기
		const float DeltaZ = ItemToCamera.Size();

		const FVector CurrentLocation = GetActorLocation();
		//x값 보간
		const float InterpXValue = FMath::FInterpTo(
			CurrentLocation.X, 
			CameraInterpLocation.X,
			DeltaTime,
			30.f);
		//Y값 보간
		const float InterpYValue = FMath::FInterpTo(
			CurrentLocation.Y,
			CameraInterpLocation.Y,
			DeltaTime,
			30.f);
		// X ,Y 아이템 위치 보간 값
		ItemLocation.X = InterpXValue;
		ItemLocation.Y = InterpYValue;
		//초기위치에 z곡선값 위치를 더해줌
		ItemLocation.Z += CurveValue * DeltaZ;
		SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);
	
		//카레마 회전 
		const FRotator CameraRotation = Character->GetCamera()->GetComponentRotation();
		//카메라 회전값이랑 초기 오피셋이랑 더해줌
		FRotator ItmeRotation(0.f, CameraRotation.Yaw + InterpInitialYawOffset, 0.f);
		SetActorRotation(ItmeRotation, ETeleportType::TeleportPhysics);

		if (ItemScaleCurve)
		{
			const float ScaleCurveValue = ItemScaleCurve->GetFloatValue(ElapsedTime);
			SetActorScale3D(FVector(ScaleCurveValue, ScaleCurveValue, ScaleCurveValue));
		}
	}
}

FVector AItem::GetInterpLocation()
{
	if (Character == nullptr) return FVector(0.f);

	switch (ItemType)
	{
	case EItemType::EIT_Ammo :
		return Character->GetInterpLocation(InterpLocIndex).SceneComponent->GetComponentLocation();
		break;

	case EItemType::EIT_Weapon :
		return Character->GetInterpLocation(0).SceneComponent->GetComponentLocation();

		break;
	

	}
	return FVector();
}

void AItem::PlayPickupSound()
{
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("3"));
		if (Character->ShouldPlayPickupSound())
		{
			UE_LOG(LogTemp, Warning, TEXT("1"));
			Character->StartPickupSoundTimer();
			if (PickupSound)
			{
				UE_LOG(LogTemp, Warning, TEXT("2"));
				UGameplayStatics::PlaySound2D(this, PickupSound);
			}
		}
	}
}

void AItem::PlayEquipSound()
{
	if (Character)
	{
		if (Character->ShouldPlayEquipSound())
		{
			Character->StartEquipSoundTimer();
			if (EquipSound)
			{
				UGameplayStatics::PlaySound2D(this, EquipSound);
			}
		}
	}
}

void AItem::SetItemState(EItemState State)
{
	ItemState = State;
	SetItemProperties(State);
}

void AItem::StartItemCurve(AMyCharacter* Char)
{
	Character = Char;

	// 보간 아이템 카운터가 가장 낮은 배열 인덱스를 가져옴 
	InterpLocIndex = Character->GetInterpLocationIndex();
	//보간 아이템 아이템 카운트 하나를 증가시켜줌
	Character->IncrementInterpLocItemCount(InterpLocIndex, 1);

	PlayPickupSound();
	//아이템 초기위치 
	ItemInterpStartLocation = GetActorLocation();
	SetItemState(EItemState::EIS_EquipInterping);
	bInterping = true;

	GetWorldTimerManager().SetTimer(
		ItemInterpTimer,
		this, &AItem::FinishInterping,
		ZCurveTime);

	//초기 Yaw 카메라
	const float CameraRotationYaw = Character->GetCamera()->GetComponentRotation().Yaw;
	
	//초기 Yaw 아이템
	const float ItmeRotationYaw = GetActorRotation().Yaw;

	// 카메라 아이템 사이의 초기 Yaw 오프셋 
	InterpInitialYawOffset = ItmeRotationYaw - CameraRotationYaw;
}

