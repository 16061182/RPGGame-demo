// Fill out your copyright notice in the Description page of Project Settings.


#include "KatanaWeapon.h"


// Sets default values
AKatanaWeapon::AKatanaWeapon() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponType = EWeaponType::WT_KATANA;
	// swingStaminaLoss = 30;
	baseDamage = 35.0f;
}

float AKatanaWeapon::GetStaminaLoss() {
	return 35.0f;
}

// Called when the game starts or when spawned
void AKatanaWeapon::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AKatanaWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
