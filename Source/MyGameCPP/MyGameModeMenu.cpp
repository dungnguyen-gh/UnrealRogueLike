// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeMenu.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMyGameModeMenu::BeginPlay()
{
    if (MainMenuWidgetClass)
    {
        if (APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0))
        {
            UUserWidget* Menu = CreateWidget<UUserWidget>(PC, MainMenuWidgetClass);
            Menu->AddToViewport();
        }
    }
}
