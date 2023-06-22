// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

UCLASS()
class SHOOTER_GAME_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category ="a",meta=(AllowPrivateAccess="true"))
	class USkeletalMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "a", meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* CollisionBox;

	/** 플에어아가 아이템을 볼떄 뜨는 위젯*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "a", meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* PickuWidget;
public:
	FORCEINLINE UWidgetComponent* GetPickupWidget() const { return PickuWidget; }
};
