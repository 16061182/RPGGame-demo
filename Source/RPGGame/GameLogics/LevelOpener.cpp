// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelOpener.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "RPGGame/Players/BasePlayer.h"


// Sets default values
ALevelOpener::ALevelOpener() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	openLevelVolume = CreateDefaultSubobject<UBoxComponent>("openLevelVolume");
	RootComponent = openLevelVolume;

	nextLevel = "Level2";

}

void ALevelOpener::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                  const FHitResult& SweepResult) {
	if (OtherActor) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			UWorld* world = GetWorld();
			if (world) {
				FName currentLevel(*world->GetMapName());
				if (currentLevel != nextLevel) {
					UGameplayStatics::OpenLevel(world, nextLevel);
				}
			}
		}
	}
}

// Called when the game starts or when spawned
void ALevelOpener::BeginPlay() {
	Super::BeginPlay();
	openLevelVolume->OnComponentBeginOverlap.AddDynamic(this, &ALevelOpener::OnOverlapBegin);

}

// Called every frame
void ALevelOpener::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}
