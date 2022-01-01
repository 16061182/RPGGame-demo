// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractive.h"
#include "InteractiveStepGear.generated.h"

UCLASS()
class RPGGAME_API AInteractiveStepGear : public ABaseInteractive {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractiveStepGear();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDownRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDownLimit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float stepTime; // 玩家踩在上面的时间
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float stepTimeLimit;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	bool bStepTimeEnough;

	UFUNCTION()
	virtual void ShowHint(); // 显示提示信息
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	FTimerHandle showHintTimer;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	bool bStopMoving; // 门已打开，本机关无效化


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult) override;
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;


};
