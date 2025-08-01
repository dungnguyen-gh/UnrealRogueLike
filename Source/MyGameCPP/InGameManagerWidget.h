// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "Components/ProgressBar.h"
#include "InGameManagerWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYGAMECPP_API UInGameManagerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent) 
	void UpdateKills(int32 NewKills);

	UFUNCTION(BlueprintImplementableEvent) 
	void UpdateHealth(float Percent);
	
	
};
