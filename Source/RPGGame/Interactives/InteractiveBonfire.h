// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGGame/Interactives/BaseInteractive.h"
#include "InteractiveBonfire.generated.h"

class UWidgetComponent;

UCLASS()
class RPGGAME_API AInteractiveBonfire : public ABaseInteractive {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractiveBonfire();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UWidgetComponent* OpenPauseHintComponent; // 打开暂停界面的提示UI

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> OpenPauseHintAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	UStaticMeshComponent* swordMeshComponent; // 额外的网格模型

	virtual void pressAction() override;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
