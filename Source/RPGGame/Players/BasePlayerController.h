// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

class UUserWidget;
UCLASS()
class RPGGAME_API ABasePlayerController : public APlayerController {
	GENERATED_BODY()

public:
	// 主UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> playerWidgetAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* playerWidget;

	// 暂停界面
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> pauseMenuAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* pauseMenuWidget;

	void ShowPauseMenu();

	void HidePauseMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	bool bPauseMenuOn;

	// 提示信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> pressE_HintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* pressE_HintWidget;

	void ShowPressE_Hint();
	void HidePressE_Hint();

	// 死亡字幕
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> playerDieHintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* playerDieHintWidget;

	void ShowPlayerDieHint();
	void HidePlayerDieHint();

	// 过场动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> cutsceneAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* cutsceneWidget;

	void ShowCutscene();
	void HideCutscene();

	// 踩踏机关提示信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> stepGearHintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* stepGearHintWidget;

	void ShowStepGearHint();
	void HideStepGearHint();

	// 踩踏机关提示信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> statueHintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* statueHintWidget;

	void ShowStatueHint();
	void HideStatueHint();

	// 击杀Boss提示信息
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> bossKilledAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* bossKilledWidget;

	void ShowBossKilledHint();
	void HideBossKilledHint();

	// 游戏胜利界面
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> winMenuAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* winMenuWidget;

	void ShowWinMenu();
	void HideWinMenu();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	bool bWinMenuOn;

	// Boss血条
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> bossHealthBarAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* bossHealthBarWidget;

	void ShowBossHealthBar();
	void HideBossHealthBar();

	// 地域名称
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> regionNameHintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UUserWidget* regionNameHintWidget;

	void ShowRegionNameHintForWhile();
	void ShowRegionNameHint();
	void HideRegionNameHint();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FTimerHandle showRegionNameHintTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	FTimerHandle hideRegionNameHintTimer;

	bool bShowRegionNameHint;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
};
