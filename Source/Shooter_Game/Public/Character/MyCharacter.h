// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "MyCharacter.generated.h"

UCLASS()
class SHOOTER_GAME_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* DefaultContext;

	/** 움직임 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MoveAction;

	/** 화면 입력 액션 */
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* LookAction;

	void Move(const FInputActionValue& Value); //캐릭터 움직이
	void Look(const FInputActionValue& Value); //캐릭터 마우스로 보기
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;
};
