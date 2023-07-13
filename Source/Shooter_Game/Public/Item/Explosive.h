// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/BulletHitInterface.h"
#include "Explosive.generated.h"

UCLASS()
class SHOOTER_GAME_API AExplosive : public AActor,public IBulletHitInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosive();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:

	/** 맞았을 때 파티클*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ExplosiveParticle;

	/** 맞았을 때 소리*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundBase* ExplosiveSound;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void BulletHit_Implementation(FHitResult HitResult) override;

};
