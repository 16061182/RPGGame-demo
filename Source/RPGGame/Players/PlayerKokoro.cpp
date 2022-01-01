// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerKokoro.h"

// Sets default values
APlayerKokoro::APlayerKokoro() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicalAttack = 100.0f;
	magicalAttack = 100.0f;
	physicalDefense = 100.0f;
	magicalDefense = 100.0f;
}

// Called when the game starts or when spawned
void APlayerKokoro::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void APlayerKokoro::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerKokoro::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
