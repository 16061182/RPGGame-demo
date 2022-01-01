// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveStepGear.h"
#include "RPGGame/Players/BasePlayer.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AInteractiveStepGear::AInteractiveStepGear() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bEnableSphere = false;
	bShowPressE_Hint = false;

	moveDown = 0.0f;
	moveDownRate = 15.0f;
	moveDownLimit = 15.0f;

	stepTime = 0.0f;
	stepTimeLimit = 1.5f;
	bStepTimeEnough = false;

	bStopMoving = false;
}

void AInteractiveStepGear::ShowHint() {
	if (inPlayerController) {
		ABasePlayer* inPlayer = Cast<ABasePlayer>(inPlayerController->GetPawn());
		if (inPlayer) {
			if (inPlayer->playerLevel < 10) {
				inPlayerController->ShowStepGearHint();
				GetWorldTimerManager().SetTimer(showHintTimer, inPlayerController,
				                                &ABasePlayerController::HideStepGearHint, 2.0f);
			}
			else {
				bStopMoving = true;
				sphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				boxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}

	}
}

// Called when the game starts or when spawned
void AInteractiveStepGear::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AInteractiveStepGear::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (!bStopMoving) {
		float shift = moveDownRate * DeltaTime;
		if (inPlayerController) {
			// 玩家踩在上面
			if (stepTime < stepTimeLimit) {
				stepTime += DeltaTime;
			}
			else {
				if (!bStepTimeEnough) {
					bStepTimeEnough = true;
					ShowHint();
				}
			}
			if (moveDown < moveDownLimit) {
				moveDown += shift;
				FVector nextLocation = GetActorLocation();
				nextLocation.Z += -shift;
				SetActorLocation(nextLocation);
			}
		}
		else {
			if (moveDown > 0) {
				moveDown -= shift;
				FVector nextLocation = GetActorLocation();
				nextLocation.Z += shift;
				SetActorLocation(nextLocation);
			}
		}
	}
}

void AInteractiveStepGear::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	stepTime = 0.0f;
	bStepTimeEnough = false;

}

void AInteractiveStepGear::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	stepTime = 0.0f;
	bStepTimeEnough = false;
}
