// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "MyGameModeCustom.h"
#include "Blueprint/UserWidget.h"


// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//Find player
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer)
	{
		auto AIController = Cast<AAIController>(GetController());
		if (AIController)
		{
			AIController->MoveToActor(TargetPlayer, 20.0f);
		}
	}
}
void AEnemy::ReceiveDamage(int32 Amount)
{
	Health = FMath::Max(Health - Amount, 0);
	if (Health <= 0) 
	{
		if (AMyGameModeCustom* GM = Cast<AMyGameModeCustom>(UGameplayStatics::GetGameMode(this)))
		{
			static int32 KillCount = 0;
			GM->InGameHUD->UpdateKills(++KillCount);
		}
		if (auto Spawner = Cast<AEnemySpawner>(GetOwner()))
		{
			Spawner->ReturnToPool(this);
		}
		else
		{
			ActivateEnemy(false);
		}
	}
}

void AEnemy::ResetHealth()
{
	Health = MaxHealth;
}

void AEnemy::ActivateEnemy(bool isActive)
{
	SetActorHiddenInGame(!isActive);
	SetActorEnableCollision(isActive);
	SetActorTickEnabled(isActive);
}


