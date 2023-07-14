// Fill out your copyright notice in the Description page of Project Settings.


#include "AIController/MyAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "../Public/Character/Enemy.h"
#include <BehaviorTree/BehaviorTree.h>

AMyAIController::AMyAIController()
{
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponet"));
	check(BlackboardComponent);

	BehaviorTreeComponet = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponet"));
	check(BehaviorTreeComponet);
}

void AMyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (InPawn == nullptr) return;

	AEnemy* Enemy = Cast<AEnemy>(InPawn);
	if (Enemy)
	{
		if (Enemy->GetBehaviorTree())
		{
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
		}
	}

}
