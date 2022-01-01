// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerAnim.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BasePlayer.h"

void UBasePlayerAnim::NativeInitializeAnimation() {
	Super::NativeInitializeAnimation();
	if (!pawn) {
		pawn = TryGetPawnOwner();
		if (pawn) {
			ownerBasePlayer = Cast<ABasePlayer>(pawn);
		}
	}
}

void UBasePlayerAnim::UpdateAnimProperties() {
	if (!pawn) {
		pawn = TryGetPawnOwner();
	}
	if (pawn) {
		if (!ownerBasePlayer) {
			ownerBasePlayer = Cast<ABasePlayer>(pawn);
		}
		const FVector playerVel = pawn->GetVelocity();
		const FVector playerVel2 = FVector(playerVel.X, playerVel.Y, 0.0f);
		moveSpeed = playerVel2.Size(); // 计算速度

		const FVector playerForwardDirection = pawn->GetActorForwardVector();
		moveSpeedForward = FVector::DotProduct(playerVel2, playerForwardDirection);
		const FVector playerRightDirection = pawn->GetActorRightVector();
		moveSpeedRight = FVector::DotProduct(playerVel2, playerRightDirection);

		bIsInAir = pawn->GetMovementComponent()->IsFalling();
	}
}
