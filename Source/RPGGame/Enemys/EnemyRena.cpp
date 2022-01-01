// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyRena.h"

#include "RPGGame/Players/BasePlayer.h"
#include "RPGGame/Players/BasePlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


// Sets default values
AEnemyRena::AEnemyRena() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->MaxWalkSpeed = 600.0f;

	physicalAttack = 120.0f;
	magicalAttack = 120.0f;
	physicalDefense = 120.0f;
	magicalDefense = 120.0f;

	maxHealth = 1500.0f;
	health = maxHealth;
	// UKismetSystemLibrary::PrintString(this, "set max health");

	soulsApply = 5000;

	bCanBeInterrupted = false; // 动作不会被打断

	bBoss = true;
}

void AEnemyRena::NotifyAnim_DieEnd() {
	Super::NotifyAnim_DieEnd();

	if (damageSource) {
		damageSource->playerController->ShowBossKilledHint();
		GetWorldTimerManager().SetTimer(showWinMenuTimer, damageSource, &ABasePlayer::ShowWinMenu, 2.0f);
	}

}

// Called when the game starts or when spawned
void AEnemyRena::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AEnemyRena::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	// UE_LOG(LogTemp, Warning, TEXT("rena maxhealth：%f"), maxHealth);


}

// Called to bind functionality to input
void AEnemyRena::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}
