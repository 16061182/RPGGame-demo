// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelOpener.generated.h"

class UBoxComponent;

UCLASS()
class RPGGAME_API ALevelOpener : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALevelOpener();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* openLevelVolume;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                            const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName nextLevel;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;


};
