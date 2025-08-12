// Fill out your copyright notice in the Description page of Project Settings.


#include "PowerUpItem.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "MainCharacter.h"

// Sets default values
APowerUpItem::APowerUpItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    RootComponent = CollisionSphere;
    CollisionSphere->InitSphereRadius(50.f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAll"));
    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APowerUpItem::OnOverlapBegin);

    MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
    MeshComponent->SetupAttachment(RootComponent);
}

void APowerUpItem::Activate(FVector SpawnLocation)
{
    SetActorLocation(SpawnLocation);
    SetActorEnableCollision(true);
    SetActorHiddenInGame(false);
    IsActive = true;
}

void APowerUpItem::Deactivate()
{
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
    IsActive = false;
}



void APowerUpItem::SetPowerUpType(EPowerUpType Type)
{
    PowerUpType = Type;

    if (!MeshComponent) return;

    switch (PowerUpType)
    {
    case EPowerUpType::HealthRecovery:
        MeshComponent->SetStaticMesh(Mesh_Health);
        break;

    case EPowerUpType::WideShot:
        MeshComponent->SetStaticMesh(Mesh_Wide);
        break;

    case EPowerUpType::NormalShot:
        MeshComponent->SetStaticMesh(Mesh_Normal);
        break;

    case EPowerUpType::ExplosionShot:
        MeshComponent->SetStaticMesh(Mesh_Explosion);
        break;

    default:
        MeshComponent->SetStaticMesh(Mesh_Normal);
        break;
    }
}

// Called when the game starts or when spawned
void APowerUpItem::BeginPlay()
{
	Super::BeginPlay();
    SetActorEnableCollision(false);
    SetActorHiddenInGame(true);
    IsActive = false;
}

void APowerUpItem::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!IsActive) return;

    if (AMainCharacter* Player = Cast<AMainCharacter>(OtherActor))
    {
        // Apply to player
        if (Player)
            Player->ApplyPowerUp(PowerUpType);

        // Return to pool via the manager (do NOT Destroy())
        if (Manager)
            Manager->ReturnToPool(this);

        else
            Deactivate();
    }
}


