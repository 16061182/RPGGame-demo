// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

class USkeletalMeshComponent;
class ABasePlayer;
class UBoxComponent;
class AController;
class USphereComponent;
class UStaticMeshComponent;
class UParticleSystemComponent;
class UParticleSystem;
class USoundCue;
class UDamageType;
class ARootCharacter;

UENUM(BlueprintType)
enum class EWeaponType : uint8 {
	WT_BASE UMETA(DisplayName = "Base"),
	WT_KATANA UMETA(DisplayName = "Katana"),
	WT_SINGLE_HAND_SWORD UMETA(DisplayName = "SingleHandSword"),
	WT_DOUBLE_HAND_SWORD UMETA(DisplayName = "DoubleHandSword"),
	WT_TWIN_SWORD UMETA(DisplayName = "TwinSword"),
	BPS_MAX UMETA(DisplayName = "DefaultMax") // 用来记录有几个枚举值
};

UENUM(BlueprintType)
enum class EDamageType : uint8 {
	DT_NORMAL UMETA(DisplayName = "Normal"),
	DT_FIRE UMETA(DisplayName = "Fire"),
	DT_THUNDER UMETA(DisplayName = "Thunder"),
	DT_WIND UMETA(DisplayName = "Wind"),
	DT_LIGHT UMETA(DisplayName = "Light"),
	DT_DARK UMETA(DisplayName = "Dark"),
	DT_MAX UMETA(DisplayName = "DefaultMax") // 用来记录有几个枚举值
};

USTRUCT(BlueprintType)
struct RPGGAME_API FDIYDamageType {
	GENERATED_BODY()

	FDIYDamageType(): baseDamage(0.0f), bIsPhysicalDamage(true), damageType(EDamageType::DT_NORMAL), bInterrupt(false),
	                  bBlowOff(false) {
	}

	FDIYDamageType(float inBaseDamage, bool inIsPhysicalDamage, EDamageType inDamageType, bool inBInterrupt,
	               bool inBBlowOff):
		baseDamage(inBaseDamage), bIsPhysicalDamage(inIsPhysicalDamage), damageType(inDamageType),
		bInterrupt(inBInterrupt), bBlowOff(inBBlowOff) {
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float baseDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsPhysicalDamage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EDamageType damageType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bInterrupt;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bBlowOff;
};

UCLASS()
class RPGGAME_API ABaseWeapon : public AActor {
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABaseWeapon();

	// 交互
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UBoxComponent* attackComponent;

	// 网格组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* staticMeshComponent;

	// 粒子系统
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UParticleSystemComponent* particleSystemComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UParticleSystem* particleSystem;

	// 声音
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundCue* swingSound;

	// 战斗系统
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_CollisionStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_CollisionEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_WeaponPlayWing();

	UFUNCTION()
	virtual void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                                  const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                                UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 属性
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float baseDamage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)

	// 控制器
	AController* weaponInstigator; // 是拿着武器的player的controller
	FORCEINLINE void SetWeaponInstigator(AController* inWeaponInstigator) { weaponInstigator = inWeaponInstigator; }

	// 所有者
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = movement)
	ARootCharacter* ownerCharacter; // 可以是player也可以是enemy
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(ARootCharacter* inOwnerCharacter, bool bEquipOnRightHand);
	bool bOwnerIsPlayer;

	// 武器类别
	EWeaponType weaponType;

	// 击中的角色列表
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	TArray<AActor*> overlapActors;

	FORCEINLINE virtual float GetStaminaLoss() { return 20.0f; }

protected:
	virtual void BeginPlay() override;

};
