// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyGameCPPGameMode.h"
#include "MyGameCPPCharacter.h"
#include "UObject/ConstructorHelpers.h"

AMyGameCPPGameMode::AMyGameCPPGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
