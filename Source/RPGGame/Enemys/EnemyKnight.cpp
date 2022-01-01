// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyKnight.h"

// Sets default values
AEnemyKnight::AEnemyKnight() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicalAttack = 110.0f;
	magicalAttack = 90.0f;
	physicalDefense = 95.0f;
	magicalDefense = 95.0f;

	maxHealth = 100.0f;
	health = maxHealth;
	
	bCanBeInterrupted = false; // 动作不会被打断

}

// Called when the game starts or when spawned
void AEnemyKnight::BeginPlay() {
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyKnight::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyKnight::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

