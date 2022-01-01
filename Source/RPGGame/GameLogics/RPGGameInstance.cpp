// Fill out your copyright notice in the Description page of Project Settings.


#include "RPGGame/GameLogics/RPGGameInstance.h"

URPGGameInstance* URPGGameInstance::GetGameInstanceInstance() {
	URPGGameInstance* instance = nullptr;
	if (GEngine) {
		const FWorldContext* context = GEngine->GetWorldContextFromGameViewport(GEngine->GameViewport);
		instance = Cast<URPGGameInstance>(context->OwningGameInstance);
	}
	return instance;
}
