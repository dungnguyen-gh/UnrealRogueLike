// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "InGameManagerWidget.h"
#include "MyGameModeCustom.generated.h"

/**
 * 
 */
UCLASS()
class MYGAMECPP_API AMyGameModeCustom : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InGameHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	virtual void BeginPlay() override;

	void ShowGameOver();

	UPROPERTY();
	UInGameManagerWidget* InGameHUD;

	
	
};
