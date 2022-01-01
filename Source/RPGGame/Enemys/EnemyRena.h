// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "EnemyRena.generated.h"

UCLASS()
class RPGGAME_API AEnemyRena : public ABaseEnemy {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyRena();

	virtual void NotifyAnim_DieEnd() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	FTimerHandle showWinMenuTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
