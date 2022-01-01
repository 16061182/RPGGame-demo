// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyGenerator.generated.h"

class UBoxComponent;

UCLASS()
class RPGGAME_API AEnemyGenerator : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEnemyGenerator();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	class UBoxComponent* spawnBoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 spawnCount;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	TSubclassOf<UUserWidget> healthBarAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Asset")
	TSubclassOf<UUserWidget> lockPointAsset;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Spawning")
	FVector GetSpawnPonit();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnAnActor(int32 level);
};
