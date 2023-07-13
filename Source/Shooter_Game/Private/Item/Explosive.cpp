// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Explosive.h"
#include <Particles/ParticleSystemComponent.h>
#include "Sound/SoundBase.h"
#include <Kismet/GameplayStatics.h>
// Sets default values
AExplosive::AExplosive()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AExplosive::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AExplosive::Tick(float DeltaTime)
{

	Super::Tick(DeltaTime);

}

void AExplosive::BulletHit_Implementation(FHitResult HitResult)
{
	if (ExplosiveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosiveSound, GetActorLocation());
	}
	if (ExplosiveParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosiveParticle,
			HitResult.Location, FRotator(0.f), true);
	}

	//데미지를 적용
	Destroy();
}

