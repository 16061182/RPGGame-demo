// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayer.h"
#include "PlayerMito.generated.h"

UCLASS()
class RPGGAME_API APlayerMito : public ABasePlayer {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerMito();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
