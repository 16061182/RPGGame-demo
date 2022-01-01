// Fill out your copyright notice in the Description page of Project Settings.


#include "SHSwordWeapon.h"


// Sets default values
ASHSwordWeapon::ASHSwordWeapon() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponType = EWeaponType::WT_SINGLE_HAND_SWORD;
	baseDamage = 30.0f;
}

float ASHSwordWeapon::GetStaminaLoss() {
	return 30.0f;
}

// Called when the game starts or when spawned
void ASHSwordWeapon::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ASHSwordWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
