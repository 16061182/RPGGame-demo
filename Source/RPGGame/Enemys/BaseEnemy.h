// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RPGGame/Roots/RootCharacter.h"
#include "BaseEnemy.generated.h"

class AAIController;
class USphereComponent;
class ABasePlayer;
class UAnimMontage;
class UBoxComponent;
class USoundCue;
class UParticleSystem;
class ABaseWeapon;
class UWidgetComponent;
class AEnemyGenerator;
class ABaseEnemyController;

UENUM(BlueprintType)
enum class EBaseEnemyState : uint8 {
	BES_IDLE UMETA(DisplayName = "Idle"),
	BES_MOVE_TO_TARGET UMETA(DisplayName = "MoveToTarget"),
	BES_ATTACK UMETA(DisplayName = "Attack"),
	BES_HURT UMETA(DisplayName = "Hurt"),
	BES_DEAD UMETA(DisplayName = "Dead"),
	BES_MAX UMETA(DisplayName = "DefaultMax") // 记录枚举数量
};

UCLASS()
class RPGGAME_API ABaseEnemy : public ARootCharacter {
	GENERATED_BODY()

public:
	ABaseEnemy();

	/******************** 基础部分 ********************/
	//----- 角色状态 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Basic")
	EBaseEnemyState enemyState;
	FORCEINLINE void SetEnemyState(EBaseEnemyState state) { enemyState = state; }
	
	//----- 敌人控制器 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Basic")
	ABaseEnemyController* enemyController;

	/******************** 战斗部分 ********************/
	//----- 攻击玩家 -----//
	// 攻击过程
	UFUNCTION(BlueprintCallable)
	void EnemyAttack();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bAttacking;
	
	// 攻击动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage") // 所有动画
	UAnimMontage* attackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage") // 所有动画
	UAnimMontage* hurtMontage;
	
	//----- 目标 -----//
	// 目标对象
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Target")
	ABasePlayer* playerTarget;

	bool bHasValidTarget; // 离开追击范围置false，进入攻击范围置true
	FORCEINLINE void SetHasValidTarget(bool hasValidTarget) { bHasValidTarget = hasValidTarget; }
	
	// 范围组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	USphereComponent* chaseSphereComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	USphereComponent* attackSphereComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
	bool bInChaseSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Range")
	bool bInAttackSphere;

	//----- 延迟进攻 -----//
	FTimerHandle attackTimer; // 延迟进攻时间间隔，下限和上限
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float minAttackTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float maxAttackTime;
	
	//----- 受击 -----//
	virtual void TakeDamageNew(FDIYDamageType DamageType, AActor* DamageCauser) override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	ABasePlayer* damageSource; // 死亡伤害来源
	
	// 死亡
	void Die();
	FORCEINLINE bool EnemyAlive() const { return enemyState != EBaseEnemyState::BES_DEAD; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	FTimerHandle deathTimer;
	float deathDelay;
	void DestroyCall(); // 销毁自己
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurt")
	USoundCue* deathSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hurt")
	int32 soulsApply; // 死亡提供魂量
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	FTimerHandle addSoulsTimer;
	float addSoulsDelay;
	void AddSouls();
	
	// 受伤
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	bool bHurting;
	
	//----- 通知 -----//
	UFUNCTION(BlueprintCallable, Category = "Notification")
	virtual void NotifyAnim_DieEnd();
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void NotifyAnim_HurtEnd();
	UFUNCTION(BlueprintCallable, Category = "Notification")
	void NotifyAnim_AttackEnd();

	//----- 锁定 -----//
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void Locked(ABasePlayer* inLockedByPlayer);
	UFUNCTION(BlueprintCallable, Category = "Lock")
	void Unlocked();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	ABasePlayer* lockedByPlayer;
	// 锁定点
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	UWidgetComponent* lockPointComponent;  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	TSubclassOf<UUserWidget> lockPointAsset;

	//----- 3D血条 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	USceneComponent* healthBarParentComponent;  // 用来控制血条跟随player视角旋转的scene component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* healthBarWidgetComponent;  
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> healthBarAsset;
	float showHealthBarTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	bool bBoss; // Boss被锁定不显示血条

	//----- 随机生成 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	AEnemyGenerator* baseEnemyGenerator;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawn")
	int32 enemyLevel;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	
	UFUNCTION()
	virtual void OnChaseSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnChaseSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnAttackSphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION()
	virtual void OnAttackSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};
