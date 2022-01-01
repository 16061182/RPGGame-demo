// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveBossTrigger.h"
#include "RPGGame/Players/BasePlayer.h"


// Sets default values
AInteractiveBossTrigger::AInteractiveBossTrigger() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bEnableSphere = false;
	bShowPressE_Hint = false;

}

// Called when the game starts or when spawned
void AInteractiveBossTrigger::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AInteractiveBossTrigger::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}

void AInteractiveBossTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                             const FHitResult& SweepResult) {
	Super::OnOverlapBegin(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (inPlayerController) {
		inPlayerController->ShowBossHealthBar();
	}
}

void AInteractiveBossTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	// Super::OnOverlapEnd(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
	if (OtherActor) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			if (player->interactiveActor && player->interactiveActor == this) {
				player->interactiveActor = nullptr;
				inPlayerController->HideBossHealthBar();
				inPlayerController = nullptr;
			}
		}
	}
}
