// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerUpTypes.h"
#include "MainCharacter.h"
#include "PowerUpManager.h"
#include "PowerUpItem.generated.h"

class USphereComponent;

UCLASS()
class MYGAMECPP_API APowerUpItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APowerUpItem();

	UFUNCTION()
	void Activate(FVector SpawnLocation);
	UFUNCTION()
	void Deactivate();

	bool IsActive = false;

	void SetManager(APowerUpManager* InManager) { Manager = InManager; }

	EPowerUpType GetPowerUpType() const { return PowerUpType; }

	UFUNCTION()
	void SetPowerUpType(EPowerUpType Type);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PowerUp")
	EPowerUpType PowerUpType;

	UPROPERTY()
	APowerUpManager* Manager;

private:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionSphere;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* Mesh_Health;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* Mesh_Wide;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* Mesh_Normal;

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	UStaticMesh* Mesh_Explosion;
	
};
