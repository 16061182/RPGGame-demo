// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ABasePlayerController::ShowPauseMenu() {
	if (regionNameHintWidget) {
		regionNameHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	if (pauseMenuWidget) {
		pauseMenuWidget->SetVisibility(ESlateVisibility::Visible);
		bShowMouseCursor = true;
		bPauseMenuOn = true;
		UGameplayStatics::SetGamePaused(this, true);
	}
}

void ABasePlayerController::HidePauseMenu() {
	if (pauseMenuWidget) {
		UGameplayStatics::SetGamePaused(this, false);
		pauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		bShowMouseCursor = false;
		bPauseMenuOn = false;
	}
}

void ABasePlayerController::ShowPressE_Hint() {
	if (pressE_HintWidget) {
		pressE_HintWidget->SetVisibility(ESlateVisibility::Visible);
		HideStatueHint();
		HideStepGearHint();
	}
}

void ABasePlayerController::HidePressE_Hint() {
	if (pressE_HintWidget) {
		pressE_HintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::ShowPlayerDieHint() {
	if (playerDieHintWidget) {
		playerDieHintWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (bossHealthBarWidget) {
		bossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::HidePlayerDieHint() {
	if (playerDieHintWidget) {
		playerDieHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::ShowCutscene() {
	if (cutsceneWidget) {
		cutsceneWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABasePlayerController::HideCutscene() {
	if (cutsceneWidget) {
		cutsceneWidget->SetVisibility(ESlateVisibility::Hidden);
		ShowRegionNameHintForWhile();
	}
}

void ABasePlayerController::ShowStepGearHint() {
	if (stepGearHintWidget) {
		stepGearHintWidget->SetVisibility(ESlateVisibility::Visible);
		HideStatueHint();
		HidePressE_Hint();
	}
}

void ABasePlayerController::HideStepGearHint() {
	if (stepGearHintWidget) {
		stepGearHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

// void ABasePlayerController::InitWidget(UUserWidget*& widget, TSubclassOf<UUserWidget>& asset, bool bShow) {
// 	if(asset) {
// 		widget = CreateWidget(this, asset);
// 		if(widget) {
// 			widget->AddToViewport();
// 			if(!bShow) {
// 				playerWidget->SetVisibility(ESlateVisibility::Hidden);
// 			} else {
// 				playerWidget->SetVisibility(ESlateVisibility::Visible);
// 			}
// 		}
// 	}
// }

void ABasePlayerController::ShowStatueHint() {
	if (statueHintWidget) {
		statueHintWidget->SetVisibility(ESlateVisibility::Visible);
		HidePressE_Hint();
		HideStepGearHint();
	}
}

void ABasePlayerController::HideStatueHint() {
	if (statueHintWidget) {
		statueHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::ShowBossKilledHint() {
	if (bossKilledWidget) {
		bossKilledWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (bossHealthBarWidget) {
		bossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::HideBossKilledHint() {
	if (bossKilledWidget) {
		bossKilledWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABasePlayerController::ShowWinMenu() {
	if (winMenuWidget) {
		winMenuWidget->SetVisibility(ESlateVisibility::Visible);
		bWinMenuOn = true;
		bShowMouseCursor = true;
		UGameplayStatics::SetGamePaused(this, true);
	}
}

void ABasePlayerController::HideWinMenu() {
	if (winMenuWidget) {
		UGameplayStatics::SetGamePaused(this, false);
		winMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		bWinMenuOn = false;
		bShowMouseCursor = false;
	}
}

void ABasePlayerController::ShowBossHealthBar() {
	if (bossHealthBarWidget) {
		bossHealthBarWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABasePlayerController::HideBossHealthBar() {
	if (bossHealthBarWidget) {
		bossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


void ABasePlayerController::ShowRegionNameHintForWhile() {
	if(!bShowRegionNameHint) {
		bShowRegionNameHint = true;
		GetWorldTimerManager().SetTimer(showRegionNameHintTimer, this, &ABasePlayerController::ShowRegionNameHint, 0.5f);
		GetWorldTimerManager().SetTimer(hideRegionNameHintTimer, this, &ABasePlayerController::HideRegionNameHint, 2.0f);
	}
	
}

void ABasePlayerController::ShowRegionNameHint() {
	if (regionNameHintWidget) {
		regionNameHintWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABasePlayerController::HideRegionNameHint() {
	if (regionNameHintWidget) {
		regionNameHintWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	bShowRegionNameHint = false;
}

void ABasePlayerController::BeginPlay() {
	Super::BeginPlay();
	if (playerWidgetAsset) {
		playerWidget = CreateWidget(this, playerWidgetAsset);
		if (playerWidget) {
			playerWidget->AddToViewport();
			playerWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (pauseMenuAsset) {
		pauseMenuWidget = CreateWidget(this, pauseMenuAsset);
		if (pauseMenuWidget) {
			pauseMenuWidget->AddToViewport();
			pauseMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	bPauseMenuOn = false;
	if (pressE_HintAsset) {
		pressE_HintWidget = CreateWidget(this, pressE_HintAsset);
		if (pressE_HintWidget) {
			pressE_HintWidget->AddToViewport();
			pressE_HintWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (playerDieHintAsset) {
		playerDieHintWidget = CreateWidget(this, playerDieHintAsset);
		if (playerDieHintWidget) {
			playerDieHintWidget->AddToViewport();
			playerDieHintWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (cutsceneAsset) {
		cutsceneWidget = CreateWidget(this, cutsceneAsset);
		if (cutsceneWidget) {
			cutsceneWidget->AddToViewport();
			cutsceneWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (stepGearHintAsset) {
		stepGearHintWidget = CreateWidget(this, stepGearHintAsset);
		if (stepGearHintWidget) {
			stepGearHintWidget->AddToViewport();
			stepGearHintWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (statueHintAsset) {
		statueHintWidget = CreateWidget(this, statueHintAsset);
		if (statueHintWidget) {
			statueHintWidget->AddToViewport();
			statueHintWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (bossKilledAsset) {
		bossKilledWidget = CreateWidget(this, bossKilledAsset);
		if (bossKilledWidget) {
			bossKilledWidget->AddToViewport();
			bossKilledWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (winMenuAsset) {
		winMenuWidget = CreateWidget(this, winMenuAsset);
		if (winMenuWidget) {
			winMenuWidget->AddToViewport();
			winMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (bossHealthBarAsset) {
		bossHealthBarWidget = CreateWidget(this, bossHealthBarAsset);
		if (bossHealthBarWidget) {
			bossHealthBarWidget->AddToViewport();
			bossHealthBarWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	if (regionNameHintAsset) {
		regionNameHintWidget = CreateWidget(this, regionNameHintAsset);
		if (regionNameHintWidget) {
			regionNameHintWidget->AddToViewport();
			regionNameHintWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	bShowRegionNameHint = false;


	// InitWidget(playerWidget, playerWidgetAsset, true);
	// InitWidget(pauseMenuWidget, pauseMenuAsset, false);
	// bPauseMenuOn = false;
	// InitWidget(pressE_HintWidget, pressE_HintAsset, false);
	// InitWidget(playerDieHintWidget, playerDieHintAsset, false);
	// InitWidget(cutsceneWidget, cutsceneAsset, false);
	// InitWidget(stepGearHintWidget, stepGearHintAsset, false);
}


void ABasePlayerController::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
}
