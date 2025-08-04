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
#include "Enemy.h"


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
	

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AMainCharacter::OnCapsuleHit);
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

	FVector SpawnLocation = FirePoint->GetComponentLocation();
	FVector Direction = FirePoint->GetForwardVector();
	ABulletProjectile* Bullet = BulletPool->GetBullet(SpawnLocation, Direction);
	if (!Bullet)
	{
		// invalid
	}
}

void AMainCharacter::OnCapsuleHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (AEnemy* E = Cast<AEnemy>(OtherActor))
	{
		ReceiveDamage(1.0f);
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

