// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractive.h"
#include "InteractiveStatue.generated.h"

UCLASS()
class RPGGAME_API AInteractiveStatue : public ABaseInteractive {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AInteractiveStatue();

	virtual void pressAction() override; // 在交互范围中，按E触发的流程

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FTimerHandle hideStatueHintTimer;

	UFUNCTION(BlueprintCallable)
	void Sink();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bStartSinking;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDownRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	float moveDownLimit;

	// 下沉时震动效果
	int32 xOffset;
	int32 yOffset;
	float shakeTimer;
	FVector centerLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


};
