// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PowerUpPoolComponent.generated.h"

class APowerUpItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYGAMECPP_API UPowerUpPoolComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPowerUpPoolComponent();

	void InitializePool(TSubclassOf<APowerUpItem> PowerUpClass, int32 PoolSize);
	APowerUpItem* GetPowerUp(const FVector& Location);
	void ReturnToPool(APowerUpItem* PowerUp);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY()
	TArray<APowerUpItem*> PowerUpPool;

	TSubclassOf<APowerUpItem> PowerUpItemClass;
		
};
