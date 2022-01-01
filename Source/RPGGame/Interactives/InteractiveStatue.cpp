// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveStatue.h"
#include "RPGGame/Players/BasePlayer.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"


// Sets default values
AInteractiveStatue::AInteractiveStatue() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bEnableSphere = false;
	bStartSinking = false;

	moveDown = 0.0f;
	moveDownRate = 200.0f;
	moveDownLimit = 5000.0f;

	xOffset = 0;
	yOffset = 0;
	shakeTimer = 0.0f;
}

void AInteractiveStatue::pressAction() {
	Super::pressAction();
	if (inPlayerController) {
		ABasePlayer* player = Cast<ABasePlayer>(inPlayerController->GetPawn());
		if (player && ((!player->bAttacking && !player->bDodging && !player->bHurting) || player->bCanCombo)) {
			// inPlayerController->HidePressE_Hint();
			inPlayerController->HidePressE_Hint();
			inPlayerController->ShowStatueHint();
			GetWorldTimerManager().SetTimer(hideStatueHintTimer, inPlayerController,
			                                &ABasePlayerController::HideStatueHint, 2.0f);
		}
	}
}


void AInteractiveStatue::Sink() {
	bStartSinking = true;

	// 取消overlap
	sphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	boxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AInteractiveStatue::BeginPlay() {
	Super::BeginPlay();
	centerLocation = GetActorLocation();

}

// Called every frame
void AInteractiveStatue::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	if (bStartSinking) {
		if (shakeTimer < 0.1f) {
			shakeTimer += DeltaTime;
		}
		else {
			shakeTimer = 0.0f;
			xOffset = FMath::RandRange(-5, 5);
			yOffset = FMath::RandRange(-5, 5);
		}
		if (moveDown < moveDownLimit) {
			moveDown += moveDownRate * DeltaTime;
			SetActorLocation(FVector(centerLocation.X + xOffset, centerLocation.Y + yOffset,
			                         centerLocation.Z - moveDown));
		}
		else {
			bStartSinking = false;
		}
	}
}

void AInteractiveStatue::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                        const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AInteractiveStatue::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}
