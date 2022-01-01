// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractiveBonfire.h"
#include "RPGGame/Players/BasePlayer.h"
#include "RPGGame/Players/BasePlayerController.h"

#include "Components/WidgetComponent.h"

// Sets default values
AInteractiveBonfire::AInteractiveBonfire() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	swordMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("swordMeshComponent"));
	swordMeshComponent->SetupAttachment(RootComponent);

	OpenPauseHintComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("OpenPauseHintComponent"));
	OpenPauseHintComponent->SetupAttachment(GetRootComponent());
	OpenPauseHintComponent->SetVisibility(false);
	OpenPauseHintComponent->SetWidgetSpace(EWidgetSpace::Screen); // 显示在屏幕上
}

void AInteractiveBonfire::pressAction() {
	Super::pressAction();
	if (inPlayerController) {
		ABasePlayer* player = Cast<ABasePlayer>(inPlayerController->GetPawn());
		if (player && ((!player->bAttacking && !player->bDodging && !player->bHurting) || player->bCanCombo)) {
			if (!inPlayerController->bPauseMenuOn) {
				inPlayerController->ShowPauseMenu();
				inPlayerController->HidePressE_Hint();
			}
		}
	}
}

// Called when the game starts or when spawned
void AInteractiveBonfire::BeginPlay() {
	Super::BeginPlay();

}

// Called every frame
void AInteractiveBonfire::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
