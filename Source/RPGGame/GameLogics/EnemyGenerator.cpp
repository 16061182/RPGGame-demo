// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyGenerator.h"
#include "RPGGame/Enemys/BaseEnemy.h"
#include "RPGGame/Enemys/BaseEnemyController.h"

#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/World.h"
#include "AIController.h"
#include "Components/WidgetComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values
AEnemyGenerator::AEnemyGenerator() {
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	spawnBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("spawnBoxComponent"));
	RootComponent = spawnBoxComponent;

	spawnCount = 3;
}

// Called when the game starts or when spawned
void AEnemyGenerator::BeginPlay() {
	Super::BeginPlay();
	int32 count = 0;
	while (count < spawnCount) { // 初始生成指定数量的actor
		SpawnAnActor(1);
		count += 1;
	}
}

// Called every frame
void AEnemyGenerator::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

}

FVector AEnemyGenerator::GetSpawnPonit() {
	FVector extent = spawnBoxComponent->GetScaledBoxExtent();
	FVector origin = spawnBoxComponent->GetComponentLocation();

	FVector point = UKismetMathLibrary::RandomPointInBoundingBox(origin, extent); // 内部随机的一个位置？
	return point;
}

void AEnemyGenerator::SpawnAnActor(int32 level) {
	UWorld* world = GetWorld();
	if (world) {
		ABaseEnemy* newEnemy = world->SpawnActor<ABaseEnemy>(ActorClass, GetSpawnPonit(), FRotator(0.0f));
		if (newEnemy) {
			newEnemy->baseEnemyGenerator = this;
			// beginplay() //好像会自动调用
			newEnemy->SpawnDefaultController();
			ABaseEnemyController* controller = Cast<ABaseEnemyController>(newEnemy->GetController());
			if (controller) {
				newEnemy->enemyController = controller;
			
			}
			// 更改初始数值
			if (level < 1) {
				level = 1;
			}
			newEnemy->enemyLevel = level;
			newEnemy->physicalAttack += (level - 1) * 3.0; // 固定的数值
			newEnemy->physicalDefense += (level - 1) * 2.0;
			newEnemy->magicalAttack += (level - 1) * 2.0;
			newEnemy->magicalDefense += (level - 1) * 2.0;
			newEnemy->maxHealth += (level - 1) * 3.0;
			// 注意：用来测试是否赋值成功的数值
			newEnemy->health = newEnemy->maxHealth;
			newEnemy->soulsApply += (level - 1) * 10;
			
			if(lockPointAsset) {
				newEnemy->lockPointComponent->SetWidgetClass(lockPointAsset);
			}

			// 设定血条材质
			if(healthBarAsset) {
				newEnemy->healthBarWidgetComponent->SetWidgetClass(healthBarAsset);
			}
		}
	}
}

