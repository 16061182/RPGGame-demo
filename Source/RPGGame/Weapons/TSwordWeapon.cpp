// Fill out your copyright notice in the Description page of Project Settings.


#include "TSwordWeapon.h"


// Sets default values
ATSwordWeapon::ATSwordWeapon() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	weaponType = EWeaponType::WT_TWIN_SWORD;
	baseDamage = 25.0f;
}

float ATSwordWeapon::GetStaminaLoss() {
	return 40.0f;
}

// Called when the game starts or when spawned
void ATSwordWeapon::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void ATSwordWeapon::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
