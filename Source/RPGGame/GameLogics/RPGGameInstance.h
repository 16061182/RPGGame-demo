// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RPGGameInstance.generated.h"

class USaveData;

UCLASS(Blueprintable, BlueprintType)
class RPGGAME_API URPGGameInstance : public UGameInstance {
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static URPGGameInstance* GetGameInstanceInstance();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USaveData* gameTempData; // 为什么只能是指针

};
