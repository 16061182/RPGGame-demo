// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerMito.h"

// Sets default values
APlayerMito::APlayerMito() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicalAttack = 100.0f;
	magicalAttack = 100.0f;
	physicalDefense = 100.0f;
	magicalDefense = 100.0f;
}

// Called when the game starts or when spawned
void APlayerMito::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void APlayerMito::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerMito::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	// 测试代码
	// AActor::GetDotProductTo();

}
