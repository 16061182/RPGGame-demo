// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RPGGame/Players/BasePlayerController.h"
#include "BaseInteractive.generated.h"

class USphereComponent;
class UBoxComponent;
class UStaticMeshComponent;
class ABasePlayer;


UCLASS()
class RPGGAME_API ABaseInteractive : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseInteractive();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	USphereComponent* sphereComponent; // 圆形可交互范围

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	bool bEnableSphere; // 启用圆形交互范围（默认启用，否则启用方形交互范围）

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	UBoxComponent* boxComponent; // 方形可交互范围

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	UStaticMeshComponent* staticMeshComponent; // 网格模型，根组件

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	UParticleSystemComponent* particleSystemComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactive")
	UParticleSystem* particleSystem;

	UFUNCTION()
	virtual void pressAction(); // 在交互范围中，按E触发的流程

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	ABasePlayerController* inPlayerController; // 保存进入其中的玩家的控制器

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactive")
	bool bShowPressE_Hint; // 进入交互范围之后，是否显示“Press E”的提示
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

};
