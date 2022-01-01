// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayer.h"

#include "AITypes.h"
#include "BasePlayerController.h"
#include "RPGGame/Weapons/BaseWeapon.h"
#include "RPGGame/Enemys/BaseEnemy.h"
#include "RPGGame/GameLogics/SaveData.h"
#include "RPGGame/Interactives/BaseInteractive.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"


// Sets default values
ABasePlayer::ABasePlayer() {
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCapsuleComponent()->InitCapsuleSize(34.f, 76.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 300.f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	cameraBoomComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("cameraBoomComponent"));
	cameraBoomComponent->SetupAttachment(RootComponent);
	cameraBoomComponent->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	cameraBoomComponent->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	followCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("followCameraComponent"));
	followCameraComponent->SetupAttachment(cameraBoomComponent, USpringArmComponent::SocketName);
	// Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	followCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	/******************** 基础部分 ********************/

	//----- 转向相关+ -----//
	baseTurnRate = 45.0f;
	baseLookUpRate = 45.0f;

	//----- 角色状态 -----//
	basePlayerState = EBasePlayerState::BPS_NORMAL;

	//----- TODO:基础数值 -----//
	strength = 1;
	agility = 1;
	toughness = 1;
	intellect = 1;

	//----- TODO:能力数值 -----//
	sprintSpeedAddition = 150.0f;


	//----- TODO:魔力相关+ -----//
	maxMagicka = 200.0f;
	magicka = maxMagicka;

	//----- 血量相关 -----//
	maxHealth = 200.0f;
	health = maxHealth;

	//----- 耐力相关+ -----//
	maxStamina = 200.0f;
	stamina = maxStamina;
	sprintStaminaLoss = 5.0f;
	staminaRecover = 40.0f;
	runningSpeed = GetCharacterMovement()->MaxWalkSpeed;
	staminaRecoverDelay = 0.3f;
	staminaRecoverDelayTimer = 0.0f;
	avoidStaminaLoss = 15.0f;
	rollStaminaLoss = 20.0f;

	//----- 移动相关+ -----//
	bForwardBackMove = false;
	bRightLeftMove = false;

	//----- 玩家控制器 -----//
	playerController = nullptr;

	/******************** 战斗部分 ********************/

	//----- 攻击按键+ -----//
	bLMouseDown = false;

	//----- 攻击敌人 -----//
	// 攻击过程
	bAttacking = false;
	attackMontage = nullptr;
	dodgeMontage = nullptr;
	hurtMontage = nullptr;

	//----- 攻击转向 -----//
	bForwardDown = false;
	bBackDown = false;
	bRightDown = false;
	bLeftDown = false;
	attackTurnDirection = FVector(0.f, 0.f, 0.f);

	//----- 目标 -----//
	// 范围组件
	senseSphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("senseSphereComponent"));
	senseSphereComponent->SetupAttachment(GetRootComponent());
	senseSphereComponent->InitSphereRadius(1000.0f);

	//----- 受击 -----//
	// 死亡
	deathSound = nullptr;
	// 受伤
	bHurting = false;


	//----- 连击系统 -----//
	bCanCombo = false;
	bHaveComboInput = false;
	nextComboPhase = 0;
	totalCombos = 2;
	bReceiveComboInput = false;

	//----- 闪避系统 -----//
	bDodgeDown = false;
	sprintDelay = 0.5f; // 连续按住空格x秒（0.2？）开始冲刺，否则翻滚
	sprintDelayTimer = 0.0f;
	sprintDelayTimer_2 = 0.0f;
	bDodging = false;
	bInvincible = false;
	bCanAvoid = true;
	bHaveRollComboInput = false;

	//----- 保存系统 -----//
	souls = 0;
	soulsNeededForNextLevel = 500;

	//----- 等级和能力值系统 -----//
	playerLevel = 1;

	//----- 等级和能力值系统 -----//
	interactiveActor = nullptr;

	//----- 游戏统计 -----//
	enemyKilled = 0;
	soulsObtained = 0;
	dieTimes = 0;

	//----- 重生位置 -----//
}


/******************** 基础部分 ********************/
//----- 角色状态 -----//
void ABasePlayer::SetPlayerState(EBasePlayerState state) {
	basePlayerState = state;
	if (basePlayerState == EBasePlayerState::BPS_SPRINT) {
		GetCharacterMovement()->MaxWalkSpeed = runningSpeed + sprintSpeedAddition;
	}
	else {
		GetCharacterMovement()->MaxWalkSpeed = runningSpeed;
	}
}

//----- 血量相关 -----//
void ABasePlayer::IncreaseHealth(float amount) {
	if (health + amount >= maxHealth) {
		health = maxHealth;
	}
	else {
		health += amount;
	}
}

void ABasePlayer::DecreaseHealth(float amount) {
	if (health - amount <= 0.0f) {
		health = 0.0f;
		Die();
	}
	else {
		health -= amount;
	}
}

//----- 耐力相关+ -----//
void ABasePlayer::IncreaseStamina(float amount) {
	if (stamina + amount >= maxStamina) {
		stamina = maxStamina;
	}
	else {
		stamina += amount;
	}
}

void ABasePlayer::DecreaseStamina(float amount) {
	if (stamina - amount <= 0.0f) {
		stamina = 0.0f;
	}
	else {
		stamina -= amount;
	}
}

/******************** 战斗部分 ********************/

//----- 攻击按键+ -----//
void ABasePlayer::SetLMouseDown() {
	bLMouseDown = true;
	// if (weaponHold) {
	if (weaponHoldLeft || weaponHoldRight) {
		PlayerAttack();
	}
}

void ABasePlayer::SetLMouseUp() {
	bLMouseDown = false;
}


//----- 攻击敌人 -----//
// 攻击过程
void ABasePlayer::PlayerAttack() {
	if (PlayerAlive()) {
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && attackMontage) {
			if ((!bAttacking && !bDodging && !bHurting) || bCanCombo) {
				if (stamina >= 1.0f) {
					// 只有有精力的时候才做动作
					// 设置连击相关属性
					bAttacking = true;
					staminaRecoverDelayTimer = 0.0f; // 播放蒙太奇之前重置精力恢复计时器
					bReceiveComboInput = false;
					bCanCombo = false;
					bHaveComboInput = false;
					bHaveRollComboInput = false;
					// bCanAvoid = true;
					sprintDelayTimer = 0.0f;
					sprintDelayTimer_2 = 0.0f;
					bDodging = false;

					switch (nextComboPhase) {
					case 0:
						animInst->Montage_Play(attackMontage);
						animInst->Montage_JumpToSection(FName("Attack1"), attackMontage); // 是异步的？似乎是
						break;
					case 1:
						animInst->Montage_Play(attackMontage);
						animInst->Montage_JumpToSection(FName("Attack2"), attackMontage);
						break;
					default:
						break;
					}
					UpdateNextComboPhase();
				}
			}
			else if (bReceiveComboInput) {
				bHaveComboInput = true;
			}
		}
	}
}

//----- 目标 -----//
FVector ABasePlayer::GetTurnDirection() const {
	FVector direction(0.0f, 0.0f, 0.0f);
	const FVector cameraForward = followCameraComponent->GetForwardVector();
	const FVector forwardDirection(cameraForward.X, cameraForward.Y, 0.0f); // 相机水平前方向
	const FVector cameraRight = followCameraComponent->GetRightVector();
	const FVector rightDirection(cameraRight.X, cameraRight.Y, 0.0f); // 相机水平右方向
	if (bForwardDown) {
		direction += forwardDirection;
	}
	if (bBackDown) {
		direction -= forwardDirection;
	}
	if (bRightDown) {
		direction += rightDirection;
	}
	if (bLeftDown) {
		direction -= rightDirection;
	}
	if (direction.Size() < 0.5f) {
		// 防止微小的扰动
		direction = FVector(0.0f, 0.0f, 0.0f);
	}
	return direction;
}

bool ABasePlayer::IsMovingToSomeDirection() const {
	FVector direction = GetTurnDirection();
	return direction.Size() > 0;
}

void ABasePlayer::UpdateAttackTurnDirection() {
	FVector direction(0.0f, 0.0f, 0.0f);
	const FVector cameraForward = followCameraComponent->GetForwardVector();
	const FVector forwardDirection(cameraForward.X, cameraForward.Y, 0.0f); // 相机水平前方向
	const FVector cameraRight = followCameraComponent->GetRightVector();
	const FVector rightDirection(cameraRight.X, cameraRight.Y, 0.0f); // 相机水平右方向
	if (bForwardDown) {
		direction += forwardDirection;
	}
	if (bBackDown) {
		direction -= forwardDirection;
	}
	if (bRightDown) {
		direction += rightDirection;
	}
	if (bLeftDown) {
		direction -= rightDirection;
	}
	if (direction.Size() < 0.5f) {
		direction = FVector(0.0f, 0.0f, 0.0f);
	}
	attackTurnDirection = direction;
}

FVector ABasePlayer::GetCameraFDirection() const {
	const FVector cameraForward = followCameraComponent->GetForwardVector();
	return FVector(cameraForward.X, cameraForward.Y, 0.0f);
}

FVector ABasePlayer::GetCameraBDirection() const {
	const FVector cameraForward = followCameraComponent->GetForwardVector();
	return FVector(-cameraForward.X, -cameraForward.Y, 0.0f);
}

FVector ABasePlayer::GetCameraRDirection() const {
	const FVector cameraRight = followCameraComponent->GetRightVector();
	return FVector(cameraRight.X, cameraRight.Y, 0.0f);
}

FVector ABasePlayer::GetCameraLDirection() const {
	const FVector cameraRight = followCameraComponent->GetRightVector();
	return FVector(-cameraRight.X, -cameraRight.Y, 0.0f);
}

//----- 受击 -----//
void ABasePlayer::TakeDamageNew(FDIYDamageType damageType, AActor* damageCauser) {
	// 用于被UGameplayStatics::ApplyDamage调用
	if (!PlayerAlive()) {
		return;
	}
	const float baseDamage = damageType.baseDamage;
	float realDamage = baseDamage;
	if (damageCauser) {
		ABaseEnemy* sourceEnemy = Cast<ABaseEnemy>(damageCauser);
		if (sourceEnemy) {
			realDamage = CalculateDamageFromCharacter(baseDamage, sourceEnemy, damageType.bIsPhysicalDamage);
		}
	}
	realDamage = int32(realDamage); // 取整
	if (health > 0.0f && health - realDamage <= 0.0f) {
		health = 0.0f;

		bHurting = false;
		bAttacking = false;

		if (damageCauser) {
			ABaseWeapon* weapon = Cast<ABaseWeapon>(damageCauser);
			if (weapon && weapon->ownerCharacter) {
				ABaseEnemy* enemy = Cast<ABaseEnemy>(weapon->ownerCharacter);
				if (enemy) {
					enemy->bHasValidTarget = false;
				}
			}
		}

		Die();
	}
	else {
		health -= realDamage;
		if (PlayerAlive() && bCanBeInterrupted) {
			if (damageType.bInterrupt && !damageType.bBlowOff) {
				// 打断动作
				const EHurtDirection hurtDirection = GetHurtDirection(damageCauser, this);
				UAnimInstance* animInst = GetMesh()->GetAnimInstance();
				if (animInst && hurtMontage) {
					ResetStateHurt();
					animInst->Montage_Play(hurtMontage);
					if (hurtDirection == EHurtDirection::HD_FRONT) {
						animInst->Montage_JumpToSection(FName("Hurt_Forward"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_BACK) {
						animInst->Montage_JumpToSection(FName("Hurt_Back"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_RIGHT) {
						animInst->Montage_JumpToSection(FName("Hurt_Right"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_LEFT) {
						animInst->Montage_JumpToSection(FName("Hurt_Left"), hurtMontage);
					}
				}

			}
			else if (damageType.bInterrupt && damageType.bBlowOff) {
				// 击倒
				const EHurtDirection hurtDirection = GetHurtDirection(damageCauser, this);
				UAnimInstance* animInst = GetMesh()->GetAnimInstance();
				if (animInst && hurtMontage) {
					ResetStateHurt();
					animInst->Montage_Play(hurtMontage);
					// animInst->Montage_JumpToSection(FName("CollapseForward"), hurtMontage);if(hurtDirection == EHurtDirection::HD_FRONT) {
					if (hurtDirection == EHurtDirection::HD_FRONT) {
						animInst->Montage_JumpToSection(FName("Collapse_Forward"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_BACK) {
						animInst->Montage_JumpToSection(FName("Collapse_Back"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_RIGHT) {
						animInst->Montage_JumpToSection(FName("Collapse_Right"), hurtMontage);
					}
					else if (hurtDirection == EHurtDirection::HD_LEFT) {
						animInst->Montage_JumpToSection(FName("Collapse_Left"), hurtMontage);
					}
				}
			}
		}
	}
}

void ABasePlayer::ResetStateHurt() {
	basePlayerState = EBasePlayerState::BPS_NORMAL;

	bForwardBackMove = false;
	bRightLeftMove = false;

	bAttacking = false;
	nextComboPhase = 0;
	bCanCombo = false;
	bReceiveComboInput = false;
	bHaveComboInput = false;
	bHaveRollComboInput = false;

	sprintDelayTimer = 0.0f;
	sprintDelayTimer_2 = 0.0f;
	bDodging = false;
	bCanAvoid = true;

	bHurting = true;

}


void ABasePlayer::Die() {
	// 播放蒙太奇接在死亡函数后？？
	if (PlayerAlive()) {
		// 设置状态
		basePlayerState = EBasePlayerState::BPS_DEAD;

		// 解除视角锁定
		UnlockTarget();

		// 播放死亡动画
		UAnimInstance* animInst = GetMesh()->GetAnimInstance();
		if (animInst && hurtMontage) {
			animInst->Montage_Play(hurtMontage);
			animInst->Montage_JumpToSection(FName("Die"), hurtMontage);
		}

		// 播放死亡音效
		if (deathSound) {
			UGameplayStatics::PlaySound2D(this, deathSound);
		}

		// 重置所有数值
		ResetStateDie();

		// 死亡次数加1
		dieTimes += 1;

		// 播放死亡字幕
		playerController->ShowPlayerDieHint();
	}
}

void ABasePlayer::ResetStateDie() {

	//----- 移动相关+ -----//
	bForwardBackMove = false;
	bRightLeftMove = false;


	/******************** 战斗部分 ********************/

	//----- 攻击按键+ -----//
	bLMouseDown = false;

	//----- 攻击敌人 -----//
	// 攻击过程
	bAttacking = false;

	//----- 攻击转向 -----//
	bForwardDown = false;
	bBackDown = false;
	bRightDown = false;
	bLeftDown = false;
	attackTurnDirection = FVector(0.f, 0.f, 0.f);

	//----- 目标 -----//
	// 范围组件

	//----- 受击 -----//
	// 受伤
	bHurting = false;


	//----- 连击系统 -----//
	bCanCombo = false;
	bHaveComboInput = false;
	nextComboPhase = 0;
	bReceiveComboInput = false;

	//----- 闪避系统 -----//
	bDodgeDown = false;
	sprintDelayTimer = 0.0f;
	sprintDelayTimer_2 = 0.0f;
	bDodging = false;
	bInvincible = false;
	bCanAvoid = true;
	bHaveRollComboInput = false;

}


void ABasePlayer::DeactiveCapsuleCollision() const {
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

/******************** 内置部分 ********************/

// Called when the game starts or when spawned
void ABasePlayer::BeginPlay() {
	Super::BeginPlay();

	resurrectLocation1 = initLocation;

	stamina = maxStamina;
	magicka = maxMagicka;

	playerController = Cast<ABasePlayerController>(GetController()); // 返回AController指针

	senseSphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	senseSphereComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	senseSphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	senseSphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	// 显示地域名称
	playerController->ShowRegionNameHintForWhile();

}

// Called every frame
void ABasePlayer::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (PlayerAlive()) {
		// 新冲刺系统
		if (bDodgeDown) {
			if (sprintDelayTimer < sprintDelay) {
				sprintDelayTimer += DeltaTime;
			}
			if (IsMovingToSomeDirection()) {
				sprintDelayTimer_2 += DeltaTime;
			}
			else {
				sprintDelayTimer_2 = 0.0f;
			}
		}
		else {
			sprintDelayTimer = 0.0f;
			sprintDelayTimer_2 = 0.0f;
		}

		// 重置奔跑状态
		if (!IsMovingToSomeDirection()) {
			SetPlayerState(EBasePlayerState::BPS_NORMAL);
		}

		if (sprintDelayTimer >= sprintDelay) {
			// 如果按下了键，就冲刺
			if ((!bAttacking && !bDodging && !bHurting) || bCanCombo) {
				if (IsMovingToSomeDirection()) {
					if (bForwardBackMove || bRightLeftMove) {
						if (stamina >= 0.1f) {
							SetPlayerState(EBasePlayerState::BPS_SPRINT);
							DecreaseStamina(sprintStaminaLoss * DeltaTime); // 冲刺耗精
							bCanAvoid = false; // 冲刺后不允许后撤
							bCanCombo = false;
							// 以下可能没用 
							bReceiveComboInput = false;
							bHaveComboInput = false;
							bHaveRollComboInput = false;
						}
						else {
							SetPlayerState(EBasePlayerState::BPS_NORMAL);
						}
					}
					else {
						SetPlayerState(EBasePlayerState::BPS_NORMAL);
					}
				}
				else if (bCanAvoid) {
					// 播放后撤蒙太奇
					staminaRecoverDelayTimer = 0.0f; // 播放蒙太奇（进行消耗精力的动作）之前重置精力恢复计时器
					UAnimInstance* animInst = GetMesh()->GetAnimInstance();
					if (animInst && dodgeMontage) {
						animInst->Montage_Play(dodgeMontage);
						animInst->Montage_JumpToSection(FName("Avoid"), dodgeMontage);
					}
					bCanAvoid = false; // 后撤一次后不允许后撤
					bAttacking = false;
					bCanCombo = false;
				}
			}
		}
		if (basePlayerState != EBasePlayerState::BPS_SPRINT) {
			if ((!bAttacking && !bDodging && !bHurting) || bCanCombo) {
				// 攻击、闪避时不回复精力
				if (staminaRecoverDelayTimer >= staminaRecoverDelay) {
					IncreaseStamina(staminaRecover * DeltaTime); // 静止回精		
				}
				else {
					staminaRecoverDelayTimer += DeltaTime;
				}
			}
		}


		if (bAttacking && attackTurnDirection.Size() > 0.5f) {
			FRotator rot = UKismetMathLibrary::MakeRotFromX(attackTurnDirection); // target - start
			FRotator rotYaw = FRotator(0.0f, rot.Yaw, 0.0f); // 水平方向
			// Current:actor相对于世界坐标的rotator，Target:目标方向的rotator（由actor指向目标）
			FRotator rotTargetPlayer = FMath::RInterpTo(GetActorRotation(), rotYaw, DeltaTime, 15.0f);
			SetActorRotation(rotTargetPlayer);
		}
		else {
			attackTurnDirection = FVector(0.0f, 0.0f, 0.0f);
		}

		// 锁定
		if (bHasTarget) {
			UpdateRotation();
		}
	}
}

void ABasePlayer::MoveForward(float Value) {
	if (!PlayerAlive()) return;
	bForwardBackMove = false;
	bForwardDown = false;
	bBackDown = false;

	if (Controller != nullptr && Value != 0.0f && !bHurting) {
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation(); // controller在世界中的旋转角度
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		// 实际上就是获得YawRotation的方向，但是由角度构成的rotator转换成方向向量需要通过先生成旋转矩阵；x轴的单位向量，通过旋转矩阵转到的方向，就是要求的方向
		AddMovementInput(Direction, Value);
		bForwardBackMove = true;

		if (Value > 0.0f) bForwardDown = true;
		else if (Value < -0.0f) bBackDown = true;
		else SetPlayerState(EBasePlayerState::BPS_NORMAL);
	}
}

void ABasePlayer::MoveRight(float Value) {
	if (!PlayerAlive()) return;
	bRightLeftMove = false;
	bRightDown = false;
	bLeftDown = false;

	if (Controller != nullptr && Value != 0.0f && !bHurting) {
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
		bRightLeftMove = true;

		if (Value >= 1.0f) bRightDown = true;
		else if (Value <= -1.0f) bLeftDown = true;
		else SetPlayerState(EBasePlayerState::BPS_NORMAL);
		// else sprintDelayTimer = 0.0f; // 注意
	}
}

void ABasePlayer::TurnAtRate(float Rate) {
	if (Rate) {
		AddControllerYawInput(Rate * baseTurnRate * GetWorld()->GetDeltaSeconds());
	}

}

void ABasePlayer::LookUpAtRate(float Rate) {
	if (Rate) {
		AddControllerPitchInput(Rate * baseLookUpRate * GetWorld()->GetDeltaSeconds());
	}
}

void ABasePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) {
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Dodge", IE_Pressed, this, &ABasePlayer::SetDodgeDown);
	PlayerInputComponent->BindAction("Dodge", IE_Released, this, &ABasePlayer::SetDodgeUp);

	PlayerInputComponent->BindAction("LMouseDown", IE_Pressed, this, &ABasePlayer::SetLMouseDown);
	PlayerInputComponent->BindAction("LMouseDown", IE_Released, this, &ABasePlayer::SetLMouseUp);

	PlayerInputComponent->BindAxis("MoveForward", this, &ABasePlayer::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABasePlayer::MoveRight);

	PlayerInputComponent->BindAxis("TurnRate", this, &ABasePlayer::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABasePlayer::LookUpAtRate);

	PlayerInputComponent->BindAction("Lock", IE_Pressed, this, &ABasePlayer::SetLockDown);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ABasePlayer::Interact);


}


void ABasePlayer::NotifyAnim_DieEnd() {
	playerController->HidePlayerDieHint();
	playerController->ShowCutscene();

	// 移动玩家位置到重生位置
	GetWorldTimerManager().SetTimer(moveToResurrectLocationTimer, this, &ABasePlayer::MovetoResurrectLocation, 1.0f);

	// 2秒后隐藏过场动画
	GetWorldTimerManager().SetTimer(cutsceneTimer, playerController, &ABasePlayerController::HideCutscene, 2.0f);

}

void ABasePlayer::NotifyAnim_HurtStart() {
	bHurting = true;
}

void ABasePlayer::NotifyAnim_HurtEnd() {
	bHurting = false;
}

void ABasePlayer::NotifyAnim_AttackEnd() {
	bAttacking = false;
	nextComboPhase = 0;
	bCanCombo = false;
	bReceiveComboInput = false;
	bHaveComboInput = false;
	bHaveRollComboInput = false;
}

void ABasePlayer::NotifyAnim_ComboStart() {
	bCanCombo = true;
	if (bHaveComboInput) {
		// 之前已经记录过输入，不等再次输入直接播放动画
		SetLMouseDown(); // 模拟点击
		SetLMouseUp();
	}
	else if (bHaveRollComboInput) {
		SetDodgeDown();
		SetDodgeUp();
	}
}

void ABasePlayer::NotifyAnim_ComboInputStart() {
	bReceiveComboInput = true;
	attackTurnDirection = FVector(0.0f, 0.0f, 0.0f);
}

void ABasePlayer::NotifyAnim_TurnStart() {
	UpdateAttackTurnDirection();
}

void ABasePlayer::NotifyAnim_DodgeStart() {
	bDodging = true;
}

void ABasePlayer::NotifyAnim_DodgeEnd() {
	bCanCombo = false;
	bDodging = false;
	nextComboPhase = 0;
	bReceiveComboInput = false;
	bHaveComboInput = false;
	bHaveRollComboInput = false;
}

void ABasePlayer::NotifyAnim_InvincibleStart() {
	bInvincible = true;
}

void ABasePlayer::NotifyAnim_InvincibleEnd() {
	bInvincible = false;
}

void ABasePlayer::NotifyAnim_CollisionStart() {
	if (weaponHoldLeft) {
		DecreaseStamina(weaponHoldLeft->GetStaminaLoss());
	}
	if (weaponHoldRight) {
		DecreaseStamina(weaponHoldRight->GetStaminaLoss());
	}

}

void ABasePlayer::NotifyAnim_CollisionEnd() {
}

void ABasePlayer::NotifyAnim_AvoidDecreaseStamina() {
	DecreaseStamina(avoidStaminaLoss);
}

void ABasePlayer::NotifyAnim_RollDecreaseStamina() {
	DecreaseStamina(rollStaminaLoss);
}

void ABasePlayer::SaveRotationMode() {
	bSave_UseControllerRotYaw = bUseControllerRotationYaw;
	bSave_OrientRotToMovement = GetCharacterMovement()->bOrientRotationToMovement;
	bSave_UseControllerDesiredRot = GetCharacterMovement()->bUseControllerDesiredRotation;
}

void ABasePlayer::LoadRotationMode() {
	bUseControllerRotationYaw = bSave_UseControllerRotYaw;
	GetCharacterMovement()->bOrientRotationToMovement = bSave_OrientRotToMovement;
	GetCharacterMovement()->bUseControllerDesiredRotation = bSave_UseControllerDesiredRot;
}

void ABasePlayer::UpdateRotation() {
	if (targetEnemy) {
		FRotator lookAtRotation = UKismetMathLibrary::FindLookAtRotation(
			GetActorLocation(), targetEnemy->GetActorLocation());
		FRotator lookAtRotation1(lookAtRotation.Pitch, lookAtRotation.Yaw, 0.0f);
		FRotator newRotation = UKismetMathLibrary::RInterpTo_Constant(GetControlRotation(), lookAtRotation1,
		                                                              UGameplayStatics::GetWorldDeltaSeconds(
			                                                              GetWorld()), 200.0f);
		GetController()->SetControlRotation(newRotation);
	}
}

void ABasePlayer::LockTarget() {
	ABaseEnemy* target = GetTargetEnemy();
	if (target) {
		bHasTarget = true;
		targetEnemy = target;
		target->Locked(this);
		// target->Locked();
		SaveRotationMode();
		GetCharacterMovement()->bOrientRotationToMovement = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
		GetController()->SetIgnoreLookInput(true);
	}
}

void ABasePlayer::UnlockTarget() {
	if (targetEnemy) {
		targetEnemy->Unlocked();
		targetEnemy = nullptr;
		bHasTarget = false;
		LoadRotationMode();
		GetController()->SetIgnoreLookInput(false);
	}
}

void ABasePlayer::SetLockDown() {
	if (bHasTarget) {
		UnlockTarget();
	}
	else {
		LockTarget();
	}
}

void ABasePlayer::LevelUp(ELevelUpType levelUPType) {
	switch (levelUPType) {
	case ELevelUpType::LUT_STRENGTH:
		strength += 1; // 只是一个象征性的等级

		physicalAttack += 3.0f;
		physicalDefense += 3.0f;
		maxHealth += 4.0f;
		break;
	case ELevelUpType::LUT_AGILITY:
		agility += 1;

		physicalAttack += 1.0f;
		magicalAttack += 1.0f;
		if (sprintSpeedAddition < 200.0f) {
			// 冲刺速度有上限
			sprintSpeedAddition += 4.0f;
			maxStamina += 4.0f;
		}
		else {
			maxStamina += 8.0f;
		}
		break;
	case ELevelUpType::LUT_TOUGHNESS:
		toughness += 1;

		physicalDefense += 3.0f;
		magicalDefense += 3.0f;
		maxHealth += 3.0f;
		maxStamina += 1.0f;
		break;
	case ELevelUpType::LUT_INTELLECT:
		intellect += 1;

		magicalAttack += 3.0f;
		magicalDefense += 3.0f;
		maxMagicka += 4.0f;
		break;
	default:
		break;
	}

	health = maxHealth;
	stamina = maxStamina;
	magicka = maxMagicka;
	playerLevel += 1;

	souls -= soulsNeededForNextLevel;
	int32 next = soulsNeededForNextLevel * 1.1;
	if (next > soulsNeededForNextLevel) {
		soulsNeededForNextLevel = next;
	}
	else {
		soulsNeededForNextLevel += 1;
	}
}

void ABasePlayer::Interact() {
	if (interactiveActor) {
		interactiveActor->pressAction();
	}
}

void ABasePlayer::PauseMenu_Resume() {
	if (playerController->bPauseMenuOn) {
		playerController->HidePauseMenu();
		if (interactiveActor && interactiveActor->bShowPressE_Hint) {
			// 仍处在可交互范围中
			playerController->ShowPressE_Hint();
		}
	}
}

void ABasePlayer::PauseMenu_LevelUpStrength() {
	if (souls >= soulsNeededForNextLevel) {
		LevelUp(ELevelUpType::LUT_STRENGTH);
	}
}

void ABasePlayer::PauseMenu_LevelUpAgility() {
	if (souls >= soulsNeededForNextLevel) {
		LevelUp(ELevelUpType::LUT_AGILITY);
	}
}

void ABasePlayer::PauseMenu_LevelUpToughness() {
	if (souls >= soulsNeededForNextLevel) {
		LevelUp(ELevelUpType::LUT_TOUGHNESS);
	}
}

void ABasePlayer::PauseMenu_LevelUpIntellect() {
	if (souls >= soulsNeededForNextLevel) {
		LevelUp(ELevelUpType::LUT_INTELLECT);
	}
}

void ABasePlayer::WinMenu_Resume() {
	if (playerController->bWinMenuOn) {
		playerController->HideWinMenu();
		if (interactiveActor && interactiveActor->bShowPressE_Hint) {
			// 仍处在可交互范围中
			playerController->ShowPressE_Hint();
		}
	}
}

void ABasePlayer::WinMenu_Quit() {
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void ABasePlayer::ShowWinMenu() {
	if (playerController) {
		playerController->HideBossKilledHint();
		playerController->ShowWinMenu();
	}
}

void ABasePlayer::MovetoResurrectLocation() {
	// 取消与交互组件的交互
	if (interactiveActor) {
		interactiveActor->inPlayerController = nullptr;
		interactiveActor = nullptr;
	}

	// 复原boss位置和血量
	if (bossEnemy) {
		bossEnemy->SetActorLocation(bossEnemy->initLocation);
		bossEnemy->SetActorRotation(bossEnemy->initRotation);
		bossEnemy->health = bossEnemy->maxHealth;
		bossEnemy->bInChaseSphere = false;
		bossEnemy->bInAttackSphere = false;
		bossEnemy = nullptr;
	}

	// 设置位置和朝向
	SetActorLocation(initLocation);
	SetActorRotation(initRotation);
	// 恢复血量等
	health = maxHealth;
	magicka = maxMagicka;
	stamina = maxStamina;
	// 设置状态
	SetPlayerState(EBasePlayerState::BPS_NORMAL);


}

void ABasePlayer::NotifyAnim_AttackStart() {
}

void ABasePlayer::SetDodgeDown() {
	bDodgeDown = true;
}

void ABasePlayer::PlayerRollOrAvoid() {
	if (stamina < 1.0f || !PlayerAlive()) return;
	if (sprintDelayTimer_2 < sprintDelay && sprintDelayTimer_2 > 0.0f || bHaveRollComboInput) {
		UE_LOG(LogTemp, Warning, TEXT("sprintDelayTimer_2 %f"), sprintDelayTimer_2);
		if ((!bAttacking && !bDodging && !bHurting) || bCanCombo) {
			// 转向翻滚方向
			if (IsMovingToSomeDirection()) {
				// 确定翻滚方向
				FRotator rot = UKismetMathLibrary::MakeRotFromX(GetTurnDirection()); // target - start
				FRotator rotYaw = FRotator(0.0f, rot.Yaw, 0.0f); // 水平方向
				SetActorRotation(rotYaw);
			}
			staminaRecoverDelayTimer = 0.0f; // 播放蒙太奇（进行消耗精力的动作之前）之前重置精力恢复计时器
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && dodgeMontage) {
				// DecreaseStamina(rollStaminaLoss);
				animInst->Montage_Play(dodgeMontage);
				animInst->Montage_JumpToSection(FName("Roll"), dodgeMontage);
			}
			bAttacking = false;
			bCanCombo = false;
			bReceiveComboInput = false;
			bHaveComboInput = false;
			bHaveRollComboInput = false;
		}
		else if (bReceiveComboInput) {
			bHaveRollComboInput = true;
		}
	}
	else if (sprintDelayTimer < sprintDelay && sprintDelayTimer > 0.0f && bCanAvoid && !IsMovingToSomeDirection()) {
		if ((!bAttacking && !bDodging && !bHurting) || bCanCombo) {
			// 播放后撤蒙太奇
			staminaRecoverDelayTimer = 0.0f; // 播放蒙太奇（进行消耗精力的动作）之前重置精力恢复计时器
			UAnimInstance* animInst = GetMesh()->GetAnimInstance();
			if (animInst && dodgeMontage) {
				// DecreaseStamina(avoidStaminaLoss);
				animInst->Montage_Play(dodgeMontage);
				animInst->Montage_JumpToSection(FName("Avoid"), dodgeMontage);
			}
			bCanAvoid = false; // 后撤一次后不允许后撤
			bAttacking = false;
			bCanCombo = false;
		}
	}
	sprintDelayTimer = 0.0f; // 注意
	sprintDelayTimer_2 = 0.0f;
	bCanAvoid = true;
}

void ABasePlayer::PauseMenu_Save() {
	USaveData* instance = Cast<USaveData>(UGameplayStatics::CreateSaveGameObject(USaveData::StaticClass()));
	if (instance) {
		instance->playerSaveData.strength = strength; // 力量 + 物理攻防
		instance->playerSaveData.agility = agility; // 敏捷 + 速度闪避
		instance->playerSaveData.toughness = toughness; // 韧性 + 双防
		instance->playerSaveData.intellect = intellect; // 智力 + 魔法攻防

		instance->playerSaveData.physicalAttack = physicalAttack; // 物理攻击
		instance->playerSaveData.magicalAttack = magicalAttack; // 魔法攻击
		instance->playerSaveData.physicalDefense = physicalDefense; // 物理防御
		instance->playerSaveData.magicalDefense = magicalDefense; // 魔法防御
		instance->playerSaveData.sprintSpeedAddition = sprintSpeedAddition;


		instance->playerSaveData.health = health;
		instance->playerSaveData.maxHealth = maxHealth;
		instance->playerSaveData.magicka = magicka;
		instance->playerSaveData.maxMagicka = maxMagicka;
		instance->playerSaveData.stamina = stamina;
		instance->playerSaveData.maxStamina = maxStamina;
		instance->playerSaveData.staminaRecover = staminaRecover;
		instance->playerSaveData.runningSpeed = runningSpeed;
		instance->playerSaveData.sprintStaminaLoss = sprintStaminaLoss; // 冲刺消耗
		instance->playerSaveData.avoidStaminaLoss = avoidStaminaLoss; // 后撤闪避消耗
		instance->playerSaveData.rollStaminaLoss = rollStaminaLoss; // 翻滚消耗

		instance->playerSaveData.souls = souls;
		instance->playerSaveData.soulsNeededForNextLevel = soulsNeededForNextLevel;
		instance->playerSaveData.playerLevel = playerLevel;
		instance->playerSaveData.basePlayerState = basePlayerState;
		instance->playerSaveData.initLocation = initLocation;
		instance->playerSaveData.initRotation = initRotation;

		instance->playerSaveData.enemyKilled = enemyKilled;
		instance->playerSaveData.soulsObtained = soulsObtained;
		instance->playerSaveData.dieTimes = dieTimes;

		if (weaponHoldClassLeft) {
			instance->playerSaveData.weaponHoldClassLeft = weaponHoldLeft->GetClass();
		}
		else {
			instance->playerSaveData.weaponHoldClassLeft = nullptr;
		}

		if (weaponHoldClassRight) {
			instance->playerSaveData.weaponHoldClassRight = weaponHoldRight->GetClass();
		}
		else {
			instance->playerSaveData.weaponHoldClassRight = nullptr;
		}

		UGameplayStatics::SaveGameToSlot(instance, TEXT("Dark Puella"), 0); //  to slot?
	}
}

void ABasePlayer::PauseMenu_Load() {
	USaveData* instance = Cast<USaveData>(UGameplayStatics::LoadGameFromSlot(TEXT("Dark Puella"), 0));
	if (instance) {
		strength = instance->playerSaveData.strength; // 力量 + 物理攻防
		agility = instance->playerSaveData.agility; // 敏捷 + 速度闪避
		toughness = instance->playerSaveData.toughness; // 韧性 + 双防
		intellect = instance->playerSaveData.intellect; // 智力 + 魔法攻防

		physicalAttack = instance->playerSaveData.physicalAttack; // 物理攻击
		magicalAttack = instance->playerSaveData.magicalAttack; // 魔法攻击
		physicalDefense = instance->playerSaveData.physicalDefense; // 物理防御
		magicalDefense = instance->playerSaveData.magicalDefense; // 魔法防御
		sprintSpeedAddition = instance->playerSaveData.sprintSpeedAddition;

		health = instance->playerSaveData.health;
		maxHealth = instance->playerSaveData.maxHealth;
		magicka = instance->playerSaveData.magicka;
		maxMagicka = instance->playerSaveData.maxMagicka;
		stamina = instance->playerSaveData.stamina;
		maxStamina = instance->playerSaveData.maxStamina;
		staminaRecover = instance->playerSaveData.staminaRecover;
		runningSpeed = instance->playerSaveData.runningSpeed;
		sprintStaminaLoss = instance->playerSaveData.sprintStaminaLoss; // 冲刺消耗
		avoidStaminaLoss = instance->playerSaveData.avoidStaminaLoss; // 后撤闪避消耗
		rollStaminaLoss = instance->playerSaveData.rollStaminaLoss; // 翻滚消耗

		souls = instance->playerSaveData.souls;
		soulsNeededForNextLevel = instance->playerSaveData.soulsNeededForNextLevel;
		playerLevel = instance->playerSaveData.playerLevel;
		basePlayerState = instance->playerSaveData.basePlayerState;
		initLocation = instance->playerSaveData.initLocation;
		initRotation = instance->playerSaveData.initRotation;

		enemyKilled = instance->playerSaveData.enemyKilled;
		soulsObtained = instance->playerSaveData.soulsObtained;
		dieTimes = instance->playerSaveData.dieTimes;

		if (weaponHoldLeft) {
			weaponHoldLeft->Destroy();
		}
		if (weaponHoldRight) {
			weaponHoldRight->Destroy();
		}
		GenerateInitWeapon(); // 重新生成武器
	}
}

void ABasePlayer::PauseMenu_Quit() {
	UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, true);
}

void ABasePlayer::PauseGame() {
	if (playerController) {
	}
}

void ABasePlayer::AddSouls(int32 soulsGain) {
	souls += soulsGain;
	soulsObtained += soulsGain;
}

ABaseEnemy* ABasePlayer::GetTargetEnemy() {
	TArray<AActor*> lockCandidateActors;
	GetOverlappingActors(lockCandidateActors, enemyFilter);
	if (lockCandidateActors.Num() == 0) {
		return nullptr;
	}
	ABaseEnemy* lockedEnemy = nullptr;
	float minDistance = 1000000.0f;
	for (auto actor : lockCandidateActors) {
		ABaseEnemy* enemyActor = Cast<ABaseEnemy>(actor);
		if (enemyActor && enemyActor->EnemyAlive()) {
			FVector dir = followCameraComponent->GetForwardVector(); // 注意是相机的前方向而不是玩家的前方向，上面写法不对
			FVector offset = enemyActor->GetActorLocation() - GetActorLocation();
			offset = offset.GetSafeNormal();
			const float dotProduct = FVector::DotProduct(dir, offset);

			if (dotProduct >= 0.5f) {
				// 在前60度角内
				float distance = GetDistanceTo(enemyActor);
				if (distance < minDistance) {
					lockedEnemy = enemyActor;
					minDistance = distance;
				}
			}
		}
	}
	return lockedEnemy;

}

void ABasePlayer::SetDodgeUp() {
	bDodgeDown = false;
	// 翻滚或后撤
	SetPlayerState(EBasePlayerState::BPS_NORMAL);
	PlayerRollOrAvoid();
}
