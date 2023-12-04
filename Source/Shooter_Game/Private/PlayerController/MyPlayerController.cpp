#include "PlayerController/MyPlayerController.h"
#include "Blueprint/UserWidget.h"

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