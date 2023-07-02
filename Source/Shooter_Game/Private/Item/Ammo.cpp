// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Ammo.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SphereComponent.h"

AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	GetCollisionBox()->SetupAttachment(RootComponent);
	GetPickupWidget()->SetupAttachment(RootComponent);
	GetAreaSphere()->SetupAttachment(RootComponent);
}

void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmo::SetItemProperties(EItemState State)
{
	Super::SetItemProperties(State);

	switch (State)
	{
	case EItemState::EIS_Pickup:
		// 아이템 속성 변경
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;
	case EItemState::EIS_EquipInterping:
		
		// 아이템 속성 변경
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_Equipped:
		// 아이템 속성 변경
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

		break;
	case EItemState::EIS_Falling:
		// 아이템 속성 변경
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		AmmoMesh->SetCollisionResponseToChannel(
			ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Block);

		break;
	case EItemState::EIS_Max:
		break;
	default:
		break;
	}
}
