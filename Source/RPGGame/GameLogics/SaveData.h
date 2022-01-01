// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RPGGame/Weapons/BaseWeapon.h"

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RPGGame/Players/BasePlayer.h"
#include "SaveData.generated.h"

/**
 * 玩家信息
 */
USTRUCT(BlueprintType)
struct RPGGAME_API FPlayerSaveData {
	GENERATED_BODY()

	//----- 能力等级 -----//
	UPROPERTY()
	int32 strength; // 力量 + 物攻，物抗，血量
	UPROPERTY()
	int32 agility; // 敏捷 + 少量物攻，少量魔攻，冲刺速度，精力
	UPROPERTY()
	int32 toughness; // 韧性 + 物抗，魔抗，血量，精力
	UPROPERTY()
	int32 intellect; // 智力 + 魔攻，魔抗，魔力

	//----- 属性面板 -----//
	UPROPERTY()
	float physicalAttack; // 物理攻击
	UPROPERTY()
	float magicalAttack; // 魔法攻击
	UPROPERTY()
	float physicalDefense; // 物理抗性
	UPROPERTY()
	float magicalDefense; // 魔法抗性
	UPROPERTY()
	float sprintSpeedAddition;
	
	//----- 状态数值 -----//
	UPROPERTY()
	float health;
	UPROPERTY()
	float maxHealth;
	UPROPERTY()
	float magicka;
	UPROPERTY()
	float maxMagicka;
	UPROPERTY()
	float stamina;
	UPROPERTY()
	float maxStamina;
	UPROPERTY()
	float staminaRecover;
	UPROPERTY()
	float runningSpeed;
	UPROPERTY()
	float sprintStaminaLoss; 
	UPROPERTY()
	float avoidStaminaLoss; 
	UPROPERTY()
	float rollStaminaLoss;
	
	//----- 武器 -----//
	UPROPERTY()
	TSubclassOf<ABaseWeapon> weaponHoldClassLeft;
	UPROPERTY()
	TSubclassOf<ABaseWeapon> weaponHoldClassRight;

	//----- 其他信息 -----//
	UPROPERTY()
	int32 souls;
	UPROPERTY()
	int32 soulsNeededForNextLevel;
	UPROPERTY()
	int32 playerLevel;
	UPROPERTY()
	EBasePlayerState basePlayerState;
	UPROPERTY()
	FVector initLocation;
	UPROPERTY()
	FRotator initRotation;
	
	//----- 游戏流程统计信息 -----//
	UPROPERTY()
	int32 enemyKilled;
	UPROPERTY()
	int32 soulsObtained;
	UPROPERTY()
	int32 dieTimes;
};

/**
 * 世界信息
 */
USTRUCT(BlueprintType)
struct FWorldSaveData {
	GENERATED_BODY()
};

UCLASS()
class RPGGAME_API USaveData : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FPlayerSaveData playerSaveData;
	UPROPERTY()
	FWorldSaveData WorldSaveData;
	
};
