// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BulletProjectile.h"
#include "BulletPoolComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SceneComponent.h"
#include "MyGameModeCustom.h"
#include "Blueprint/UserWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "NiagaraFunctionLibrary.h"
#include "Engine/Engine.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create spring arm
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1000.0f;
	CameraBoom->SetRelativeRotation(FRotator(-60.0f, 0.0f, 0.0f));
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->bInheritPitch = false;

	// Create follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom);
	FollowCamera->bUsePawnControlRotation = false;

	// Enable character rotation from controller
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Create FirePoint 
	FirePoint = CreateDefaultSubobject<USceneComponent>(TEXT("FirePoint"));
	FirePoint->SetupAttachment(RootComponent); 
	FirePoint->SetRelativeLocation(FVector(60, 0, 0));
	FirePoint->SetRelativeRotation(FRotator::ZeroRotator);

	BulletPool = CreateDefaultSubobject<UBulletPoolComponent>("BulletPool");
	BulletPool->BulletClass = BulletClass;

	DamageSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DamageSphere"));
	DamageSphere->SetupAttachment(RootComponent);
	DamageSphere->SetSphereRadius(100.0f); // Tune radius as needed
	DamageSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DamageSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DamageSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DamageSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (BulletPool && BulletClass)
	{
		BulletPool->BulletClass = BulletClass;
	}

	BulletPool->InitializePool();
	
	// Add Input Mapping Context
	if (APlayerController* PC = Cast<APlayerController>(Controller))
	{
		PC->bShowMouseCursor = true;
		PC->bEnableClickEvents = true;
		PC->bEnableMouseOverEvents = true;
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	CurrentHealth = MaxHealth;
	

	DamageSphere->OnComponentBeginOverlap.AddDynamic(this, &AMainCharacter::OnDamageSphereOverlap);
}


// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult Hit;
		if (PC->GetHitResultUnderCursorByChannel(
			UEngineTypes::ConvertToTraceType(ECC_Visibility), 
			false, 
			Hit))
		{
			FVector Target = Hit.ImpactPoint;
			FVector ToCursor = Target - GetActorLocation();
			ToCursor.Z = 0;

			if (!ToCursor.IsNearlyZero()) 
			{
				FRotator WantRot = ToCursor.Rotation();
				WantRot.Pitch = 0;
				WantRot.Roll = 0;
				SetActorRotation(WantRot);
			}
		}
	}
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EIC->BindAction(ShootAction, ETriggerEvent::Started, this, &AMainCharacter::HandleShoot);
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	FVector2D input = Value.Get<FVector2D>();
	
	if (input.IsNearlyZero()) return;

	const FVector WorldForward = FVector::ForwardVector; // X axis
	const FVector WorldRight = FVector::RightVector;
	AddMovementInput(WorldForward, input.Y);
	AddMovementInput(WorldRight, input.X);

	/*const FRotator YawRot(0, GetActorRotation().Yaw, 0);
	const FVector Forward = FRotationMatrix(YawRot).GetUnitAxis(EAxis::X);
	const FVector Right = FRotationMatrix(YawRot).GetUnitAxis(EAxis::Y);
	AddMovementInput(Forward, input.Y);
	AddMovementInput(Right, input.X);*/
}

void AMainCharacter::HandleShoot(const FInputActionValue& Value)
{
	ShootBullet();
}



void AMainCharacter::ShootBullet()
{
	if (!BulletPool || !FirePoint) return;

	const FVector SpawnLocation = FirePoint->GetComponentLocation();
	const FRotator BaseRot = FirePoint->GetComponentRotation();
	const FVector Right = FirePoint->GetRightVector();


	auto SpawnShot = [&](const FRotator& ShotRot, const FVector& Pos)
		{
			const FVector Dir = ShotRot.Vector().GetSafeNormal(); // convert rotator -> direction
			// debug arrow
			DrawDebugDirectionalArrow(GetWorld(), Pos, Pos + Dir * 300.f, 40.f, FColor::Red, false, 2.0f, 0, 4.0f);
			BulletPool->GetBullet(Pos, Dir);
		};


	switch (CurrentPowerUp)
	{
		case EPowerUpType::WideShot:
		{
			const float AngleOffset = 30.0f;
			const float SideOffset = 30.0f;

			FRotator LeftRot = BaseRot + FRotator(0.f, -AngleOffset, 0.f);
			FRotator MiddleRot = BaseRot;
			FRotator RightRot = BaseRot + FRotator(0.f, AngleOffset, 0.f);

			// Offset spawn positions sideways
			FVector LeftSpawn = SpawnLocation - Right * SideOffset;
			FVector MiddleSpawn = SpawnLocation;
			FVector RightSpawn = SpawnLocation + Right * SideOffset;

			SpawnShot(LeftRot, LeftSpawn);
			SpawnShot(MiddleRot, MiddleSpawn);
			SpawnShot(RightRot, RightSpawn);
			break;
		}
		case EPowerUpType::NormalShot:
		case EPowerUpType::HealthRecovery:
		case EPowerUpType::ExplosionShot:
		default:
			SpawnShot(BaseRot, SpawnLocation);
			break;
	}

	
}

void AMainCharacter::RecoverHealth(float Amount)
{
	CurrentHealth = FMath::Clamp(CurrentHealth + Amount, 0.f, MaxHealth);
	if (auto GM = Cast<AMyGameModeCustom>(UGameplayStatics::GetGameMode(this)))
	{
		GM->InGameHUD->UpdateHealth(CurrentHealth / MaxHealth);
	}
}

void AMainCharacter::OnDamageSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AEnemy* Enemy = Cast<AEnemy>(OtherActor)) 
	{
		ReceiveDamage(5.0f);
	}
}



void AMainCharacter::ReceiveDamage(float Amount)
{
	CurrentHealth = FMath::Max(0.f, CurrentHealth - Amount);
	if (auto GM = Cast<AMyGameModeCustom>(UGameplayStatics::GetGameMode(this)))
	{
		GM->InGameHUD->UpdateHealth(CurrentHealth / MaxHealth);
		if (CurrentHealth <= 0.f)
			GM->ShowGameOver();
	}
}

void AMainCharacter::ApplyPowerUp(EPowerUpType Type)
{
	switch (Type)
	{
		case EPowerUpType::HealthRecovery:
			RecoverHealth(25.0f);
			break;

		case EPowerUpType::WideShot:
			CurrentPowerUp = Type;
			break;

		case EPowerUpType::NormalShot:
			CurrentPowerUp = Type;
			break;

		case EPowerUpType::ExplosionShot:
			{
				FVector ExplosionLocation = GetActorLocation();
				float ExplosionRadius = 300.f;
				int32 ExplosionDamage = 3;

				// draw debug sphere
				DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 16, FColor::Red, false, 2.0f);

				TArray<FOverlapResult> Overlaps;
				FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplosionRadius);

				bool bHit = GetWorld()->OverlapMultiByChannel(
					Overlaps,
					ExplosionLocation,
					FQuat::Identity,
					ECC_Pawn,
					SphereShape
				);

				if (bHit)
				{
					for (auto& Overlap : Overlaps)
					{
						if (AEnemy* Enemy = Cast<AEnemy>(Overlap.GetActor()))
						{
							Enemy->ReceiveDamage(ExplosionDamage);
						}
					}
				}

				// Play effect
				if (ExplosionEffect)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ExplosionEffect, ExplosionLocation);
				}

				break;
			}
	}
}

