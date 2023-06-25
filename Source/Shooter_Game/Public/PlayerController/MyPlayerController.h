// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SHOOTER_GAME_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AMyPlayerController();
protected:
	virtual void BeginPlay() override;

private:
	/**UHDOverlayClass클레스 참조 */
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Widget,meta=(AllowPrivateAccess="true"))
	TSubclassOf<class UUserWidget> UHDOverlayClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UUserWidget* HUDOverlay;
};
