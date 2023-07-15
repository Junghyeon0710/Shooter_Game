// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../Interface/BulletHitInterface.h"
#include "Enemy.generated.h"

UCLASS()
class SHOOTER_GAME_API AEnemy : public ACharacter , public IBulletHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent) //부모를 호출 시켜줘야됨
	void ShowHealthBar();
	void ShowHealthBar_Implementation(); //부모

	UFUNCTION(BlueprintImplementableEvent) // 블루프린트 또는 레벨 블루프린트에서 구현
	void HideHealthBar();

	void PlayHitMontage(FName Section,float PlayRate =1.0f);
	UFUNCTION(BlueprintCallable)
	void PlayAttackMontage(FName Section, float PlayRate = 1.0f);

	void ResetHitReactTimer();

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber, FVector Location);

	UFUNCTION()
	void DestoryNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	UFUNCTION()
	void AgroSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void AgroSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UFUNCTION(BlueprintCallable)
	void SetStunned(bool Stunned);

	UFUNCTION()
	void CombatRangeOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void CombatRangeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintPure)
	FName GetAttackSectionName();

	UFUNCTION()
	void OnLeftWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRightWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//무기 충돌 활성화/비활성화
	UFUNCTION(BlueprintCallable)
	void ActivateLeftWeapon();
	UFUNCTION(BlueprintCallable)
	void DeActivateLeftWeapon();

	UFUNCTION(BlueprintCallable)
	void ActivateRightWeapon();
	UFUNCTION(BlueprintCallable)
	void DeActivateRightWeapon();

	void DoDamage(class AMyCharacter* Victim);

	void SpawnBlood(AMyCharacter* Victime, FName SocketName);

	// 캐릭터를 스턴시키기
	void StunCharacter(AMyCharacter* Victime);

	void ResetCanAttack();

	void Die();

	UFUNCTION(BlueprintCallable)
	void FinishDeath();

private:

	/** 맞았을 때 파티클*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class UParticleSystem* ImpactParticles;


	/** 맞았을 때 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	class USoundBase* ImpactSound;

	/**적 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float Health = 100.f;
	
	/**최대 체력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	/** 헤드 본 이름*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	FString HeadBone;

	/** 헬스바 사라지는 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HealthBarDisplayTime = 4.f;

	FTimerHandle HealthBarTimer;

	/** 맞을때 애니메이션*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* HitMontage;

	FTimerHandle HitReactTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitReactTimeMin = .5f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitRactTimeMax = 3.f;

	bool bCanHitReact = true;

	UPROPERTY(VisibleAnywhere, Category = Combat, meta = (AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	/** 데미지 사라지는 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float HitNumberDestroyTime = 1.5f;

	/** 비에버트리 클래스 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true"))
	class UBehaviorTree* BeHaviorTree;

	/** 순찰 장소 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true",MakeEditWidget ="true"))
	FVector PatrolPoint;

	/** 순찰 장소 */
	UPROPERTY(EditAnywhere, Category = "Behavior Tree", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FVector PatrolPoint2;

	UPROPERTY()
	class AMyAIController* EnemyController;

	/* 스페어에 닿으면 쫒아옴 **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	class USphereComponent* AgrosSphere;

	/** 맞앗을때 실행 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bStunned = false;

	/** 스턴 찬스 0 = 0퍼 1 = 100퍼*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float StunChance =.5f;

	/** 공겸범위 안에 있나 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bInAttackRange = false;

	/** 공격범위 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	USphereComponent* CombatRangeSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	class UAnimMontage* AttackMontage;

	FName AttackLFast = "AttackL_Fast";
	FName AttackRFast = "AttackR_Fast";

	FName AttackL = "AttackL";
	FName AttackR = "AttackR";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	class UBoxComponent* LeftWeaponCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	class UBoxComponent* RightWeaponCollision;

	/** 적 기본 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float BaseDamage = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FName LeftWeaponSocket = "Fx_trail_L_01";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	FName RightWeaponSocket = "Fx_trail_R_01";

	/** 적이 공격할 수 있나 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
	bool bCanAttack = true;

	FTimerHandle AttackWaitTimer;

	/** 적이 다음공격까지 걸리는 시간*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float AttackWaitTime =1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	class UAnimMontage* DeathMontage;

	bool bDying = false;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	float DeathTime;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void BulletHit_Implementation(FHitResult HitResult, AActor* Shooter, AController* ShooterController) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	
public:
	FORCEINLINE FString GetHeadBone() const { return HeadBone; }

	UFUNCTION(BlueprintImplementableEvent)
	void ShowHitNumber(int32 Damage, FVector HitLoaction,bool bHeadShot);

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BeHaviorTree; }
};
