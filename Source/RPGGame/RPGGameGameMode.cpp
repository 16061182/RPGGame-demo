// Copyright Epic Games, Inc. All Rights Reserved.

#include "RPGGameGameMode.h"
#include "RPGGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ARPGGameGameMode::ARPGGameGameMode() {
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL) {
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
