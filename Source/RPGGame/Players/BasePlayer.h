#pragma once

#include "CoreMinimal.h"
#include "RPGGame/Roots/RootCharacter.h"
#include "BasePLayer.generated.h"


class USoundCue;
class UParticleSystem;
class ABaseEnemy;
class USphereComponent;
class USpringArmComponent;
class UCameraComponent;
class ABasePlayerController;
class ABaseWeapon;
class ABaseInteractive;

/*
 * 玩家状态枚举
 */
UENUM(BlueprintType)
enum class EBasePlayerState : uint8 {
	BPS_NORMAL UMETA(DisplayName = "Normal"),
	BPS_SPRINT UMETA(DisplayName = "Sprint"),
	BPS_ATTACK UMETA(DisplayName = "Attack"),
	BPS_HURT UMETA(DisplayName = "Hurt"),
	BPS_DEAD UMETA(DisplayName = "Dead"),
	BPS_MAX UMETA(DisplayName = "DefaultMax") // 用来记录有几个枚举值
};


/*
 * Combo类型枚举（目前没用到）
 */
UENUM(BlueprintType)
enum class EComboType : uint8 {
	CT_DEFAULT UMETA(DisplayName = "Default"),
	CT_ATTACK UMETA(DisplayName = "Attack"),
	CT_AVOID UMETA(DisplayName = "Avoid"),
	CT_ROLL UMETA(DisplayName = "Roll")
};

/*
 * 升级时选择提升的属性枚举
 */
UENUM(BlueprintType)
enum class ELevelUpType : uint8 {
	LUT_STRENGTH UMETA(DisplayName = "Strength"),
	LUT_AGILITY UMETA(DisplayName = "Agility"),
	LUT_TOUGHNESS UMETA(DisplayName = "Toughness"),
	LUT_INTELLECT UMETA(DisplayName = "Intellect")
};

UCLASS()
class RPGGAME_API ABasePlayer : public ARootCharacter {
	GENERATED_BODY()
	//----- 相机+ -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* cameraBoomComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	// 虽然是private，但是在蓝图里可以读
	UCameraComponent* followCameraComponent;

public:
	ABasePlayer();

	/******************** 基础部分 ********************/

	//----- 转向相关+ -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float baseTurnRate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	float baseLookUpRate;

	//----- 角色状态 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Basic")
	EBasePlayerState basePlayerState;
	void SetPlayerState(EBasePlayerState state); // 同时设置最大行走速度

	//----- 基础数值 -----//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	int32 strength; // 力量 + 物理攻防
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	int32 agility; // 敏捷 + 速度闪避
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	int32 toughness; // 韧性 + 双防
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attribute")
	int32 intellect; // 智力 + 魔法攻防

	//----- 能力数值 -----// 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Ability")
	float sprintSpeedAddition; // 冲刺速度补正

	//----- 血量相关 -----//
	void IncreaseHealth(float amount);
	void DecreaseHealth(float amount);

	//----- TODO:魔力相关+ -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Magicka")
	float magicka;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Magicka")
	float maxMagicka;

	//----- 耐力相关+ -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float stamina;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float maxStamina;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float staminaRecover;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina")
	float runningSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float sprintStaminaLoss; // 冲刺消耗
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float avoidStaminaLoss; // 后撤闪避消耗
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina")
	float rollStaminaLoss; // 翻滚消耗

	float staminaRecoverDelay; // 耐力回复停顿时间
	float staminaRecoverDelayTimer; // 耐力回复停顿时间计时器

	void IncreaseStamina(float amount);
	void DecreaseStamina(float amount);

	//----- 移动相关（向前或向右移动时才能冲刺）+ -----//
	bool bForwardBackMove;
	bool bRightLeftMove;

	//----- 玩家控制器 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player Data")
	ABasePlayerController* playerController;

	/******************** 战斗部分 ********************/

	//----- 攻击按键+ -----//
	bool bLMouseDown;
	virtual void SetLMouseDown(); // 按下左键:捡起武器或攻击 // 与ShiftDown()不同，触发性动作的逻辑写在Down函数里
	virtual void SetLMouseUp();
	FORCEINLINE bool GetLMouseDown() const { return bLMouseDown; }


	//----- 攻击敌人 -----//
	// 攻击过程
	void PlayerAttack();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	bool bAttacking;


	// 攻击动画
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage") // 所有动画
	UAnimMontage* attackMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage") // 所有动画
	UAnimMontage* dodgeMontage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Montage") // 所有动画
	UAnimMontage* hurtMontage;


	// 范围组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sense")
	USphereComponent* senseSphereComponent; // 感知组件

	virtual void TakeDamageNew(FDIYDamageType DamageType, AActor* DamageCauser) override;
	void ResetStateHurt();

	// 死亡
	void Die();
	FORCEINLINE bool PlayerAlive() const { return basePlayerState != EBasePlayerState::BPS_DEAD; }
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Die")
	USoundCue* deathSound;
	void ResetStateDie();

	// 受伤
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	bool bHurting;

	// 计时器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Hurt")
	FTimerHandle deactiveCollisionTimer; // ?
	void DeactiveCapsuleCollision() const; // ?

	//----- 连击系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bCanCombo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bHaveComboInput;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bReceiveComboInput; // 开始接收Combo输入
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bComboInuput; // 是否有Combo输入
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 nextComboPhase; // 下一次连击的动画
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	int32 totalCombos; // 连击动画数
	FORCEINLINE void UpdateNextComboPhase() { nextComboPhase = (nextComboPhase + 1) % totalCombos; }

	//----- 攻击转向系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bForwardDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bBackDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bRightDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bLeftDown;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	FVector attackTurnDirection;

	void UpdateAttackTurnDirection(); // 通过设置变量实现插值转身
	FVector GetTurnDirection() const; // 用于瞬时转身
	FVector GetCameraFDirection() const;
	FVector GetCameraBDirection() const;
	FVector GetCameraRDirection() const;
	FVector GetCameraLDirection() const;


	//----- 闪避系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	bool bDodgeDown; // 是否按下冲刺键【shift】（用来在按下时间里保持冲刺状态）
	// float dodgeTime; // 闪避无敌时间，前面已经定义过
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float sprintDelay; // 连续按住空格x秒（0.2？）开始冲刺，否则翻滚
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float sprintDelayTimer; // 记录空格按下的时间
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	float sprintDelayTimer_2; //  记录空格+方向键按下的时间
	FORCEINLINE void SetDodgeDown(); // 持续性动作秩序改变状态，逻辑写在tick里
	FORCEINLINE void SetDodgeUp();
	FORCEINLINE bool GetDodgeDown() const { return bDodgeDown; }
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	bool bDodging; // 是否在后撤或者翻滚过程中，后撤和翻滚都可以连击
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	bool bInvincible;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dodge")
	bool bCanAvoid; // 在不松开空格的情况下，后撤一次或者开始冲刺之后就不再允许后撤
	bool IsMovingToSomeDirection() const; // 按下的方向键是否可以朝某方向移动，例如W+S一起按就会返回false，按下三个键就会返回true
	void PlayerRollOrAvoid();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combo")
	bool bHaveRollComboInput; // 与bHaveComboInput相同，只不过标志翻滚输入

	//----- 游戏保存和加载 -----//
	UFUNCTION(BlueprintCallable)
	void PauseGame();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 souls;
	UFUNCTION()
	void AddSouls(int32 soulsGain);
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 soulsNeededForNextLevel;

	//----- 锁定系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bHasTarget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	ABaseEnemy* targetEnemy;
	UFUNCTION(BlueprintCallable)
	ABaseEnemy* GetTargetEnemy(); // 玩家感知范围内，前方45度角的最近的敌人
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lock")
	TSubclassOf<ABaseEnemy> enemyFilter;
	UFUNCTION(BlueprintCallable)
	void LockTarget();
	UFUNCTION(BlueprintCallable)
	void UnlockTarget();

	void SetLockDown(); // 按下左键:捡起武器或攻击 // 与ShiftDown()不同，触发性动作的逻辑写在Down函数里

	// 记录旋转方式
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bSave_UseControllerRotYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bSave_OrientRotToMovement;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock")
	bool bSave_UseControllerDesiredRot;
	UFUNCTION(BlueprintCallable)
	void SaveRotationMode();
	UFUNCTION(BlueprintCallable)
	void LoadRotationMode();
	UFUNCTION(BlueprintCallable)
	void UpdateRotation();


	// //----- 游戏逻辑 -----//

	//----- 等级和能力值系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 playerLevel;
	UFUNCTION(BlueprintCallable)
	void LevelUp(ELevelUpType levelUPType);

	//----- 交互系统 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interact")
	ABaseInteractive* interactiveActor;
	UFUNCTION(BlueprintCallable)
	void Interact();
	// 暂停界面
	UFUNCTION(BlueprintCallable)
	void PauseMenu_Resume();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_Save();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_Load();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_Quit();
	// 升级
	UFUNCTION(BlueprintCallable)
	void PauseMenu_LevelUpStrength();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_LevelUpAgility();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_LevelUpToughness();
	UFUNCTION(BlueprintCallable)
	void PauseMenu_LevelUpIntellect();
	// 游戏胜利界面
	UFUNCTION(BlueprintCallable)
	void WinMenu_Resume();
	UFUNCTION(BlueprintCallable)
	void WinMenu_Quit();
	UFUNCTION(BlueprintCallable)
	void ShowWinMenu();


	//----- 游戏统计 -----//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Statistics")
	int32 enemyKilled;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Statistics")
	int32 soulsObtained;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game Statistics")
	int32 dieTimes;

	//----- 死亡重生 -----//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resurrect") // 自动设置为player start位置
	FVector resurrectLocation1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resurrect")
	FRotator resurrectRotation1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resurrect") // 需要在编辑器里手动设置为第二个篝火附近位置
	FVector resurrectLocation2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resurrect")
	FRotator resurrectRotation2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resurrect")
	FTimerHandle cutsceneTimer; // 过场动画计时器
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Resurrect")
	FTimerHandle moveToResurrectLocationTimer; // 移动回重生点计时器
	UFUNCTION(BlueprintCallable, Category = "Resurrect")
	void MovetoResurrectLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resurrect")
	ABaseEnemy* bossEnemy; // 用于被boss击杀后，复原boss的位置并恢复其体力（在蓝图里设置）


	//----- notify函数，通过蒙太奇的notify机制触发 -----//
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_AttackStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_DieEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_HurtStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_HurtEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_AttackEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_ComboStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_ComboInputStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_TurnStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_DodgeStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_DodgeEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_InvincibleStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_InvincibleEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_CollisionStart();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_CollisionEnd();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_AvoidDecreaseStamina();
	UFUNCTION(BlueprintCallable)
	void NotifyAnim_RollDecreaseStamina();


protected:
	virtual void BeginPlay() override;

	// 角色控制
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	// virtual void Jump() override; // 回调函数的函数指针调用虚函数

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// 返回相机
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return cameraBoomComponent; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return followCameraComponent; }
};
