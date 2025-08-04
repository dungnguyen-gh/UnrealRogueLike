// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeCustom.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"


void AMyGameModeCustom::BeginPlay()
{
	Super::BeginPlay();
	if (InGameHUDClass)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
		{
			InGameHUD = CreateWidget<UInGameManagerWidget>(PC, InGameHUDClass);
			InGameHUD->AddToViewport();
		}
	}
}

void AMyGameModeCustom::ShowGameOver()
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
	{
		if (InGameHUD) InGameHUD->RemoveFromParent();
		if (GameOverWidgetClass)
		{
			UUserWidget* GO = CreateWidget<UUserWidget>(PC, GameOverWidgetClass);
			GO->AddToViewport();
		}
	}
}
