

// Fill out your copyright notice in the Description page of Project Settings.

#include "Blueprint/UserWidget.h"


// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerController/MyPlayerController.h"

AMyPlayerController::AMyPlayerController()
{

}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	if (UHDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(GetWorld(), UHDOverlayClass);
		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}

	}
}