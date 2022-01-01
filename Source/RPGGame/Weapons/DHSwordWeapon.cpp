// Fill out your copyright notice in the Description page of Project Settings.

#include "DHSwordWeapon.h"


// Sets default values
ADHSwordWeapon::ADHSwordWeapon() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponType = EWeaponType::WT_DOUBLE_HAND_SWORD;
	baseDamage = 40.0f;
}

float ADHSwordWeapon::GetStaminaLoss() {
	return 40.0f;
}

// Called when the game starts or when spawned
void ADHSwordWeapon::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ADHSwordWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
