// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "RPGGame/Weapons/BaseWeapon.h"
#include "RootCharacter.generated.h"

class ABaseWeapon;
class USoundCue;

UENUM(BlueprintType)
enum class EHurtDirection : uint8 {
	HD_FRONT UMETA(DisplayName = "Front"),
	HD_BACK UMETA(DisplayName = "Back"),
	HD_RIGHT UMETA(DisplayName = "Right"),
	HD_LEFT UMETA(DisplayName = "Left")
};

UCLASS()
class RPGGAME_API ARootCharacter : public ACharacter {
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ARootCharacter();

	//----- 能力数值 -----//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float physicalAttack; // 物理攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float magicalAttack; // 魔法攻击
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float physicalDefense; // 物理防御
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float magicalDefense; // 魔法防御

	//----- 血量相关 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	float health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float maxHealth;
	UFUNCTION(BlueprintCallable, Category = "Health")
	void RecoverHealth();

	//----- 武器 -----//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<ABaseWeapon> weaponHoldClassLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	TSubclassOf<ABaseWeapon> weaponHoldClassRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	ABaseWeapon* weaponHoldLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	ABaseWeapon* weaponHoldRight;
	void SetWeaponHold(ABaseWeapon* inWeaponHold, bool onRightHand);
	// 初始化武器
	virtual void GenerateInitWeapon();

	//----- 受击效果 -----//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	USoundCue* hitSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UParticleSystem* hitParticle;

	//----- 一些为了方便调用所以写在这里的空函数 -----//
	virtual void TakeDamageNew(FDIYDamageType damageType, AActor* damageCauser);

	//----- 方向获取 -----//（不包括z轴）
	FVector GetCharacterFDirection() const;
	FVector GetCharacterBDirection() const;
	FVector GetCharacterRDirection() const;
	FVector GetCharacterLDirection() const;

	//----- 受击方向获取 -----//
	UFUNCTION(BlueprintCallable)
	EHurtDirection GetHurtDirection(AActor* from, AActor* to);

	//----- 计算伤害 -----// （计算RootCharacter之间互相造成的伤害）
	UFUNCTION(BlueprintCallable)
	float CalculateDamageFromCharacter(float baseDamage, ARootCharacter* damageCauser, bool isPhysical);

	//----- 是否可以被普通攻击打断或击倒 -----//（这个设定过于简单了，原本计划实现类黑魂的韧性系统）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanBeInterrupted;

	// 初始位置
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
	FVector initLocation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Init")
	FRotator initRotation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};
