// Fill out your copyright notice in the Description page of Project Settings.


#include "RootCharacter.h"

#include "Kismet/KismetMathLibrary.h"
#include "RPGGame/Weapons/BaseWeapon.h"


// Sets default values
ARootCharacter::ARootCharacter() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	physicalAttack = 100.0f;
	magicalAttack = 100.0f;
	physicalDefense = 100.0f;
	magicalDefense = 100.0f;

	maxHealth = 100.0f;

	weaponHoldLeft = nullptr;

	hitSound = nullptr;
	hitParticle = nullptr;

	bCanBeInterrupted = true;
}


void ARootCharacter::RecoverHealth() {
	health = maxHealth;
}

void ARootCharacter::SetWeaponHold(ABaseWeapon* inWeaponHold, bool onRightHand) {
	if (onRightHand) {
		if (weaponHoldRight) {
			weaponHoldRight->Destroy();
		}
		weaponHoldRight = inWeaponHold;
	}
	else {
		if (weaponHoldLeft) {
			weaponHoldLeft->Destroy();
		}
		weaponHoldLeft = inWeaponHold;
	}
}


void ARootCharacter::GenerateInitWeapon() {
	if (weaponHoldClassLeft) {
		ABaseWeapon* initWeapon = GetWorld()->SpawnActor<ABaseWeapon>(weaponHoldClassLeft, FVector(), FRotator());
		if (initWeapon) {
			initWeapon->EquipWeapon(this, false);
		}
	}
	if (weaponHoldClassRight) {
		ABaseWeapon* initWeapon = GetWorld()->SpawnActor<ABaseWeapon>(weaponHoldClassRight, FVector(), FRotator());
		if (initWeapon) {
			initWeapon->EquipWeapon(this, true);
		}
	}
}


void ARootCharacter::TakeDamageNew(FDIYDamageType DamageType, AActor* DamageCauser) {

}

FVector ARootCharacter::GetCharacterFDirection() const {
	const FVector actorForward = GetActorForwardVector();
	return FVector(actorForward.X, actorForward.Y, 0.0f);
}

FVector ARootCharacter::GetCharacterBDirection() const {
	const FVector actorForward = GetActorForwardVector();
	return FVector(-actorForward.X, -actorForward.Y, 0.0f);
}

FVector ARootCharacter::GetCharacterRDirection() const {
	const FVector actorRight = GetActorRightVector();
	return FVector(actorRight.X, actorRight.Y, 0.0f);
}

FVector ARootCharacter::GetCharacterLDirection() const {
	const FVector actorRight = GetActorRightVector();
	return FVector(-actorRight.X, -actorRight.Y, 0.0f);
}

EHurtDirection ARootCharacter::GetHurtDirection(AActor* from, AActor* to) {
	EHurtDirection res;
	if (from && to) {
		const float dotProduct1 = to->GetDotProductTo(from); // 调用者的方向向量点乘标准化的位移向量
		if (dotProduct1 >= 0.707f) {
			// 前
			res = EHurtDirection::HD_FRONT;
		}
		else if (dotProduct1 <= -0.707f) {
			// 后
			res = EHurtDirection::HD_BACK;
		}
		else {
			const FVector unitDirection = UKismetMathLibrary::GetDirectionUnitVector(
				to->GetActorLocation(), from->GetActorLocation());
			const float dotProduct2 = FVector::DotProduct(to->GetActorRightVector(), unitDirection);
			if (dotProduct2 >= 0.0f) {
				res = EHurtDirection::HD_RIGHT;
			}
			else {
				res = EHurtDirection::HD_LEFT;
			}
		}
	}
	else {
		res = EHurtDirection::HD_FRONT;
	}
	return res;
}

float ARootCharacter::CalculateDamageFromCharacter(float baseDamage, ARootCharacter* damageCauser, bool isPhysical) {
	float damage = baseDamage;
	float attack;
	float defense;
	if (isPhysical) {
		attack = damageCauser->physicalAttack;
		defense = physicalDefense;
	}
	else {
		attack = damageCauser->magicalAttack;
		defense = magicalDefense;
	}
	// 伤害逻辑
	if (attack <= defense) {
		damage *= (attack / defense);
	}
	else {
		damage += (attack - defense);
	}
	return damage;
}


// Called when the game starts or when spawned
void ARootCharacter::BeginPlay() {
	Super::BeginPlay();
	health = maxHealth;

	GenerateInitWeapon();

	initLocation = GetActorLocation();
	initRotation = GetActorRotation();
}

// Called every frame
void ARootCharacter::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARootCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
