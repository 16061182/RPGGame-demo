// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseWeapon.h"
#include "SHSwordWeapon.generated.h"

UCLASS()
class RPGGAME_API ASHSwordWeapon : public ABaseWeapon {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASHSwordWeapon();
	virtual float GetStaminaLoss() override;

	// virtual EWeaponType GetWeaponType() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
