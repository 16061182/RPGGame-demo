// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemyAnim.h"
#include "BaseEnemy.h"

void UBaseEnemyAnim::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	if (!pawn) {
		pawn = TryGetPawnOwner();
		if (pawn) {
			ownerBaseEnemy = Cast<ABaseEnemy>(pawn);
		}
	}
}

void UBaseEnemyAnim::UpdateAnimProperties() {
	if (!pawn) {
		pawn = TryGetPawnOwner();
	}
	if (pawn) {
		if (!ownerBaseEnemy) {
			ownerBaseEnemy = Cast<ABaseEnemy>(pawn);
		}
		FVector playerVel = pawn->GetVelocity();
		FVector playerVel2 = FVector(playerVel.X, playerVel.Y, 0.0f);
		moveSpeed = playerVel2.Size();

		const FVector playerForwardDirection = pawn -> GetActorForwardVector();
		moveSpeedForward = FVector::DotProduct(playerVel2, playerForwardDirection);
		const FVector playerRightDirection = pawn -> GetActorRightVector();
		moveSpeedRight = FVector::DotProduct(playerVel2, playerRightDirection);
	}
}
