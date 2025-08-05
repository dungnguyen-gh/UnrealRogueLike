// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
#include "AIController.h"
#include "Components/CapsuleComponent.h"
#include "MyGameModeCustom.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = AAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Block);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	ExplosionEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ExplosionEffect"));
	ExplosionEffect->SetupAttachment(RootComponent);
	ExplosionEffect->bAutoActivate = false;

	GetCharacterMovement()->MaxWalkSpeed = 250.0f;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	//Find player
	TargetPlayer = UGameplayStatics::GetPlayerPawn(this, 0);
	CachedAIController = Cast<AAIController>(GetController());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPlayer && CachedAIController)
	{
		CachedAIController->MoveToActor(TargetPlayer, 20.0f);
	}
}
void AEnemy::PlayExplosionAndReturnToPool()
{
	// Stop movement
	if (CachedAIController)
	{
		CachedAIController->StopMovement();
	}
	// Disable collision
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetVisibility(false, true);

	if (ExplosionEffect)
	{
		ExplosionEffect->SetWorldLocation(GetActorLocation());
		ExplosionEffect->Activate(true);
	}
	const float EffectDuration = 1.0f;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_ExplosionReturn,
		[this]()
		{
			if (auto Spawner = Cast<AEnemySpawner>(GetOwner()))
			{
				Spawner->ReturnToPool(this);
			}
		},
		EffectDuration,
		false
	);
}
void AEnemy::ReceiveDamage(int32 Amount)
{
	Health = FMath::Max(Health - Amount, 0);
	if (Health <= 0) 
	{
		if (AMyGameModeCustom* GM = Cast<AMyGameModeCustom>(UGameplayStatics::GetGameMode(this)))
		{
			static int32 KillCount = 0;
			if (GM->InGameHUD)
			{
				GM->InGameHUD->UpdateKills(++KillCount);
			}
		}
		PlayExplosionAndReturnToPool();
	}
}

void AEnemy::ResetHealth()
{
	Health = MaxHealth;
}

void AEnemy::ActivateEnemy(bool isActive)
{
	if (isActive)
	{
		// Reactivate mesh and collision
		GetMesh()->SetVisibility(true, true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ResetHealth();

		// Ensure AI controller is cached again after pooling
		if (!CachedAIController)
		{
			CachedAIController = Cast<AAIController>(GetController());
		}
	}

	SetActorHiddenInGame(!isActive);
	SetActorEnableCollision(isActive);
	SetActorTickEnabled(isActive);
}


