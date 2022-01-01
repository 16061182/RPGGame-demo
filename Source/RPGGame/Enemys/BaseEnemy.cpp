// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "RPGGame/Players/BasePlayer.h"
#include "RPGGame/Weapons/BaseWeapon.h"
#include "RPGGame/GameLogics/EnemyGenerator.h"
#include "BaseEnemyController.h"

#include "AIController.h"
#include "Components/SphereComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"

// Sets default values
ABaseEnemy::ABaseEnemy() {
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;

	/******************** 基础部分 ********************/
	//----- 角色状态 -----//
	enemyState = EBaseEnemyState::BES_IDLE;
	
	//----- 敌人控制器 -----//
	enemyController = nullptr;

	/******************** 战斗部分 ********************/

	//----- 攻击玩家 -----//
	// 攻击过程
	bAttacking = false;
	// 攻击动画
	attackMontage = nullptr;
	hurtMontage = nullptr;

	//----- 目标 -----//
	// 目标对象
	playerTarget = nullptr;
	bHasValidTarget = false; // 离开追击范围置false，进入攻击范围置true

	// 范围组件
	chaseSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("chaseSphereComponent"));
	chaseSphereComponent->SetupAttachment(GetRootComponent());
	chaseSphereComponent->InitSphereRadius(600.0f);
	attackSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("attackSphereComponent"));
	attackSphereComponent->SetupAttachment(GetRootComponent());
	attackSphereComponent->InitSphereRadius(75.0f);
	bInChaseSphere = false;
	bInAttackSphere = false;

	//----- 延迟进攻 -----//
	minAttackTime = 0.2f;
	maxAttackTime = 0.5f;

	//----- 受击 -----//
	damageSource = nullptr;
	// 死亡
	deathDelay = 5.0f;
	deathSound = nullptr;
	soulsApply = 220;
	addSoulsDelay = 1.0f;
	// 受伤
	bHurting = false;

	//----- 锁定 -----//
	lockPointComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("lockPointComponent"));
	lockPointComponent->SetupAttachment(GetRootComponent());
	lockPointComponent->SetVisibility(false);
	lockPointComponent->SetWidgetSpace(EWidgetSpace::Screen); // 显示在屏幕上
	lockedByPlayer = nullptr;

	//----- 3D血条 -----//
	healthBarParentComponent = CreateDefaultSubobject<USceneComponent>(TEXT("healthBarParentComponent"));
	healthBarParentComponent->SetupAttachment(GetRootComponent());
	healthBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("healthBarWidgetComponent"));
	healthBarWidgetComponent->SetupAttachment(healthBarParentComponent);
	healthBarWidgetComponent->SetVisibility(false);
	healthBarWidgetComponent->SetWidgetSpace(EWidgetSpace::World); // 是否是显示在游戏里？
	showHealthBarTimer = 0.0f;
	bBoss = false;

	//----- 随机生成 -----//
	baseEnemyGenerator = nullptr;
	enemyLevel = 1;
}

/******************** 基础部分 ********************/


/******************** 战斗部分 ********************/

//----- 攻击玩家 -----//
void ABaseEnemy::EnemyAttack() {
	if(EnemyAlive()) {
		if (enemyController) {
			enemyController->StopMovement(); // 注意攻击之前停止移动
			SetEnemyState(EBaseEnemyState::BES_ATTACK);
		}
		if (playerTarget && playerTarget->PlayerAlive()) {
			if (!bAttacking && !bHurting) {
				bAttacking = true;
				UAnimInstance* animInst = GetMesh()->GetAnimInstance();
				if (animInst && attackMontage) {
					int32 section = FMath::RandRange(0, 2); // 下面的用法是正确的
					switch (section) {
					case 0:
						animInst->Montage_Play(attackMontage);
						animInst->Montage_JumpToSection(FName("Attack1"), attackMontage);
						break;
					case 1:
						animInst->Montage_Play(attackMontage);
						animInst->Montage_JumpToSection(FName("Attack2"), attackMontage);
						break;
					case 2:
						animInst->Montage_Play(attackMontage);
						animInst->Montage_JumpToSection(FName("Attack3"), attackMontage);
						break;
					default:
						break;
					}
				}
			}
			
		}
	}
}


//----- 受击 -----//
void ABaseEnemy::AddSouls() {
	damageSource->AddSouls(soulsApply);
	damageSource->enemyKilled += 1;
	// player->AddSouls(soulsApply);
}

void ABaseEnemy::TakeDamageNew(FDIYDamageType damageType, AActor* damageCauser) {
	const float baseDamage = damageType.baseDamage;
	float realDamage = baseDamage;
	if (damageCauser) {
		ABasePlayer* sourcePlayer = Cast<ABasePlayer>(damageCauser);
		if(sourcePlayer) {
			realDamage = CalculateDamageFromCharacter(baseDamage, sourcePlayer, damageType.bIsPhysicalDamage);
		}
	}
	realDamage = int32(realDamage); // 取整
	if (health - realDamage <= 0.0f) {
		health = 0.0f;
		SetEnemyState(EBaseEnemyState::BES_DEAD);
		
		bHurting = false;
		bAttacking = false;
		
		if(damageCauser) {
			ABasePlayer* player = Cast<ABasePlayer>(damageCauser);
			if(player) {
				damageSource = player;
				GetWorldTimerManager().SetTimer(addSoulsTimer, this, &ABaseEnemy::AddSouls, addSoulsDelay); // 0.8秒后调用函数
			}
		}
		
		Die();
	}
	else {
		health -= realDamage;

		// 受击显示血条
		showHealthBarTimer = 0.0f;
		if (healthBarAsset && !bBoss) {
			healthBarWidgetComponent->SetVisibility(true);
		}
		
		if(EnemyAlive() && bCanBeInterrupted) {
			if (damageType.bInterrupt && !damageType.bBlowOff) { // 打断动作，打断效果通过蒙太奇slot的优先级机制实现
				bHurting = true; 
				bAttacking = false; 
				UAnimInstance* animInst = GetMesh()->GetAnimInstance();
				if (animInst && hurtMontage) {
					animInst->Montage_Play(hurtMontage);
					animInst->Montage_JumpToSection(FName("HurtForward"), hurtMontage);
				}
				
			} else if(damageType.bInterrupt && damageType.bBlowOff) { // 击倒
				bHurting = true; 
				bAttacking = false;
				// TODO:获得伤害来源的方向，调转actor的方向为面对它
				UAnimInstance* animInst = GetMesh()->GetAnimInstance();
				if (animInst && hurtMontage) {
					animInst->Montage_Play(hurtMontage);
					animInst->Montage_JumpToSection(FName("CollapseForward"), hurtMontage);
				}
			}
		}
	}
}

void ABaseEnemy::Die() {
	// 取消锁定
	if(lockedByPlayer) {
		lockedByPlayer->UnlockTarget();
		lockedByPlayer = nullptr;
	}
	
	// 播放动画
	UAnimInstance* animInst = GetMesh()->GetAnimInstance();
	if (animInst && hurtMontage) {
		animInst->Montage_Play(hurtMontage);
		animInst->Montage_JumpToSection(FName("Die"), hurtMontage);
	}

	// 取消角色碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	chaseSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackSphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if (weaponHoldLeft) {
		weaponHoldLeft->staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weaponHoldLeft->attackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		weaponHoldLeft->staticMeshComponent->SetVisibility(false, true); // +++
	}
	if (weaponHoldRight) {
		weaponHoldRight->staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		weaponHoldRight->attackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
		weaponHoldRight->staticMeshComponent->SetVisibility(false, true); // +++
	}

	// 播放死亡音效
	if (deathSound) {
		UGameplayStatics::PlaySound2D(this, deathSound);
	}

}

void ABaseEnemy::DestroyCall() {
	if(baseEnemyGenerator) {// 死了，再生成一个
		if(damageSource) {
			baseEnemyGenerator->SpawnAnActor(damageSource->playerLevel); // 根据player等级确定新生成Enemy的属性 
		} else {
			baseEnemyGenerator->SpawnAnActor(1);
		}
		
	}
	if(weaponHoldLeft) {
		weaponHoldLeft->Destroy();
	}
	if(weaponHoldRight) {
		weaponHoldRight->Destroy();
	}
	Destroy();
}


/******************** 内置部分 ********************/
// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay() {
	Super::BeginPlay();
	enemyController = Cast<ABaseEnemyController>(GetController()); // controller初始化写在beginplay中

	// 设置碰撞
	chaseSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	chaseSphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	chaseSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	chaseSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	attackSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	attackSphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	attackSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	attackSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 设置重叠事件
	chaseSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnChaseSphereOverlapBegin);
	chaseSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnChaseSphereOverlapEnd);
	attackSphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnAttackSphereOverlapBegin);
	attackSphereComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnAttackSphereOverlapEnd);
	

	//取消跟相机的碰撞
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	//敌人漫游随机种子
	double timeMs = FTimespan(FDateTime::Now().GetTicks()).GetTotalMilliseconds();
	int32 timeSeed = (int32)((int64)timeMs % INT_MAX);
	FMath::RandInit(timeSeed);

	// 设置锁定光点材质
	if(lockPointAsset) {
		lockPointComponent->SetWidgetClass(lockPointAsset);
	}

	// 设定血条材质
	if(healthBarAsset) {
		healthBarWidgetComponent->SetWidgetClass(healthBarAsset);
	}
}

// Called every frame
void ABaseEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if(!bBoss) {
		if(showHealthBarTimer >= 0.0f && showHealthBarTimer < 2.0f) {
			showHealthBarTimer += DeltaTime;
		} else if(showHealthBarTimer >= 2.0f) {
			if (healthBarAsset && !lockedByPlayer) {
				healthBarWidgetComponent->SetVisibility(false);
			}
			showHealthBarTimer = -1.0;
		}
	}
}

// Called to bind functionality to input
void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::OnChaseSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                           const FHitResult& SweepResult) {
	if (OtherActor && EnemyAlive()) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			if (!player->PlayerAlive()) return;
			playerTarget = player;
			bInChaseSphere = true;
		}
	}
}

void ABaseEnemy::OnChaseSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor && EnemyAlive()) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			bHasValidTarget = false; // 离开追击范围置false，进入攻击范围置true
			if (!player->PlayerAlive()) return;
			

			playerTarget = nullptr;
			bInChaseSphere = false;
		}
	}
}

void ABaseEnemy::OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                            const FHitResult& SweepResult) {
	if (OtherActor && EnemyAlive()) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			bHasValidTarget = true;
			if (!player->PlayerAlive()) return;
			bInAttackSphere = true;
			EnemyAttack();
		}
	}
}

void ABaseEnemy::OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor && EnemyAlive()) {
		ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
		if (player) {
			if (!player->PlayerAlive()) return;

			bInAttackSphere = false;
		}
		GetWorldTimerManager().ClearTimer(attackTimer); 
	}
}

void ABaseEnemy::NotifyAnim_DieEnd() {
	GetMesh()->bPauseAnims = true;
	GetMesh()->bNoSkeletonUpdate = true;
	GetMesh()->SetVisibility(false);

	GetWorldTimerManager().SetTimer(deathTimer, this, &ABaseEnemy::DestroyCall, deathDelay); // 隔一段时间后销毁
}

void ABaseEnemy::NotifyAnim_HurtEnd() {
	bHurting = false;

	// 这个逻辑可能是调试的时候发现敌人被打之后不会还手而后加的，因为如果玩家持续处于attack sphere内的话就不会出发进入时间而导致敌人攻击，所以要重新调用攻击
	if (playerTarget && playerTarget->PlayerAlive()) {
		if (bInAttackSphere && EnemyAlive() && bHasValidTarget) {
			EnemyAttack();
		}
	}
}

void ABaseEnemy::NotifyAnim_AttackEnd() {
	bAttacking = false;
	if (bInAttackSphere && EnemyAlive() && bHasValidTarget) {
		float attackRandTime = FMath::FRandRange(minAttackTime, maxAttackTime); // 延迟一段时间后重新调用EnemyAttack
		GetWorldTimerManager().SetTimer(attackTimer, this, &ABaseEnemy::EnemyAttack, attackRandTime);
	}
}

void ABaseEnemy::Locked(ABasePlayer* inLockedByPlayer) {
	if (lockPointAsset){
		lockPointComponent->SetVisibility(true);
	}
	if (healthBarAsset && !bBoss) {
		healthBarWidgetComponent->SetVisibility(true);
	}
	if(inLockedByPlayer) {
		lockedByPlayer = inLockedByPlayer;
	}
}

void ABaseEnemy::Unlocked() {
	if (lockPointAsset){
		lockPointComponent->SetVisibility(false);
	}
	if (healthBarAsset && !bBoss) {
		healthBarWidgetComponent->SetVisibility(false);
	}
	lockedByPlayer = nullptr;
}



