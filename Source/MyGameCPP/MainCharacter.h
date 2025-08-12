// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BulletPoolComponent.h"
#include "PowerUpTypes.h"
#include "NiagaraSystem.h"
#include "MainCharacter.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class USceneComponent;
class UBulletPoolComponent;
class USphereComponent;

UCLASS()
class MYGAMECPP_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Movement functions
	void Move(const FInputActionValue& Value);

	void HandleShoot(const FInputActionValue& Value);
	void ShootBullet();
	void RecoverHealth(float Amount);

	UFUNCTION()
	void OnDamageSphereOverlap(
		UPrimitiveComponent* OverlappedComponent, 
		AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, 
		bool bFromSweep, 
		const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PowerUps")
	EPowerUpType CurrentPowerUp = EPowerUpType::NormalShot;

	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraSystem* ExplosionEffect;

public:
	

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* ShootAction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	USceneComponent* FirePoint;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TSubclassOf<class ABulletProjectile> BulletClass;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UBulletPoolComponent* BulletPool;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth = 100.0f;

	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	USphereComponent* DamageSphere;


	void ReceiveDamage(float Amount);

	void ApplyPowerUp(EPowerUpType Type);

	
};
