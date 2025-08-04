// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class MYGAMECPP_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();
	void ReceiveDamage(int32 Amount);

	void ResetHealth();
	bool IsAlive() const { return Health > 0; }

	void ActivateEnemy(bool isActive);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	APawn* TargetPlayer;

private:
	int32 Health = 3;

	int32 MaxHealth = 3;
};
