// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractive.h"
#include "RPGGame/Players/BasePlayer.h"
#include "RPGGame/Players/BasePlayerController.h"

#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABaseInteractive::ABaseInteractive() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("staticMeshComponent"));
	RootComponent = staticMeshComponent;
	
	sphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComponent"));
	sphereComponent->SetupAttachment(RootComponent);

	boxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent"));
	boxComponent->SetupAttachment(RootComponent);

	particleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("particleSystemComponent"));
	particleSystemComponent->SetupAttachment(RootComponent);

	bEnableSphere = true;
	particleSystem = nullptr;
	inPlayerController = nullptr;
	bShowPressE_Hint = true;
}

void ABaseInteractive::pressAction() {
}

// Called when the game starts or when spawned
void ABaseInteractive::BeginPlay() {
	Super::BeginPlay();

	if(bEnableSphere) {
		sphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractive::OnOverlapBegin);
		sphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseInteractive::OnOverlapEnd);
	
		sphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
		sphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		sphereComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		sphereComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);	
	} else {
		boxComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseInteractive::OnOverlapBegin);
		boxComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseInteractive::OnOverlapEnd);
	
		boxComponent->SetCollisionObjectType(ECC_WorldDynamic);
		boxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		boxComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
		boxComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
}

// Called every frame
void ABaseInteractive::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

void ABaseInteractive::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	if(OtherActor) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if(player) {
			if(!player->interactiveActor) {
				player->interactiveActor = this;
				inPlayerController = player->playerController;
				if(bShowPressE_Hint) {
					inPlayerController->ShowPressE_Hint();	
				}
			}
		}
	}
}

void ABaseInteractive::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if(OtherActor) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if(player) {
			if(player->interactiveActor && player->interactiveActor == this) {
				if(bShowPressE_Hint) {
					inPlayerController->HidePressE_Hint();	
				}
				player->interactiveActor = nullptr;
				inPlayerController = nullptr;
			}
		}
	}
}

