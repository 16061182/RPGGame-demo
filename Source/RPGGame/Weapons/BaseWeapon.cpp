// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "RPGGame/Roots/RootCharacter.h"
#include "RPGGame/Enemys/BaseEnemy.h"
#include "RPGGame/Players/BasePlayer.h"


#include "Components/BoxComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "RPGGame/RPGGame.h"


ABaseWeapon::ABaseWeapon() {
	PrimaryActorTick.bCanEverTick = true;

	staticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("staticMeshComponent"));
	RootComponent = staticMeshComponent;

	attackComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("attackComponent"));
	attackComponent->SetupAttachment(GetRootComponent());

	particleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("particleSystemComponent"));
	particleSystemComponent->SetupAttachment(RootComponent);

	particleSystem = nullptr;
	swingSound = nullptr;
	baseDamage = 10.0f;

	ownerCharacter = nullptr;
	bOwnerIsPlayer = false;

	weaponType = EWeaponType::WT_BASE;

	overlapActors.Empty();
}

void ABaseWeapon::BeginPlay() {
	Super::BeginPlay();

	attackComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseWeapon::OnAttackOverlapBegin);
	attackComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseWeapon::OnAttackOverlapEnd);

	//设置武器的碰撞 // ?
	attackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	attackComponent->SetCollisionObjectType(ECC_DamageCauser);
	attackComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	staticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseWeapon::NotifyAnim_CollisionStart() {
	attackComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly); // ???
	attackComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	attackComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	overlapActors = TArray<AActor*>();
}

void ABaseWeapon::NotifyAnim_CollisionEnd() {
	attackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseWeapon::NotifyAnim_WeaponPlayWing() {
	if (swingSound) {
		UGameplayStatics::PlaySound2D(this, swingSound);
	}
}

void ABaseWeapon::OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                       UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                       const FHitResult& SweepResult) {
	if (OtherActor) {
		if (bOwnerIsPlayer) {
			// TODO：添加伤害计算
			ABaseEnemy* enemy = Cast<ABaseEnemy>(OtherActor);
			if (enemy) {
				AActor* enemyActor = enemy;
				if (!overlapActors.Contains(enemyActor)) {
					// 是否已经击中过
					overlapActors.Emplace(enemyActor);
					// 播放血液飞溅粒子特效
					if (enemy->hitParticle) {
						UGameplayStatics::SpawnEmitterAtLocation(
							this,
							enemy->hitParticle, staticMeshComponent->GetSocketLocation(FName("SwordHoldSocket")),
							FRotator(0.0f, 0.0f, 0.0f),
							true);
					}
					// 播放打击音效
					if (enemy->hitSound) {
						UGameplayStatics::PlaySound2D(this, enemy->hitSound);
					}
					// 造成伤害
					FDIYDamageType damageType(baseDamage, true, EDamageType::DT_NORMAL, true, false);
					enemy->TakeDamageNew(damageType, this->ownerCharacter);
				}
			}
		}
		else {
			ABasePlayer* player = Cast<ABasePlayer>(OtherActor);
			if (player && !player->bInvincible) {
				// 不处于闪避无敌时间
				AActor* playerActor = player;
				if (!overlapActors.Contains(playerActor)) {
					overlapActors.Emplace(playerActor);
					// 播放血液飞溅粒子特效
					if (player->hitParticle) {
						UGameplayStatics::SpawnEmitterAtLocation(
							this,
							player->hitParticle, staticMeshComponent->GetSocketLocation(FName("SwordHoldSocket")),
							FRotator(0.0f, 0.0f, 0.0f),
							true);
					}
					// 播放打击音效
					if (player->hitSound) {
						UGameplayStatics::PlaySound2D(this, player->hitSound);
					}
					// 造成伤害
					FDIYDamageType damageType(baseDamage, true, EDamageType::DT_NORMAL, true, false);
					player->TakeDamageNew(damageType, this->ownerCharacter);
				}
			}
		}
	}
}

void ABaseWeapon::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                     UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor) {
		ABaseEnemy* enemy = Cast<ABaseEnemy>(OtherActor);
		if (enemy) {

		}
	}
}

void ABaseWeapon::EquipWeapon(ARootCharacter* inOwnerCharacter, bool bEquipOnRightHand) {
	if (inOwnerCharacter) {
		const USkeletalMeshSocket* meshSocket;
		if (bEquipOnRightHand) {
			// 装备在左手还是右手
			meshSocket = inOwnerCharacter->GetMesh()->GetSocketByName("RightHandSocket");
		}
		else {
			meshSocket = inOwnerCharacter->GetMesh()->GetSocketByName("LeftHandSocket");
		}

		if (meshSocket) {
			SetWeaponInstigator(inOwnerCharacter->GetController());
			meshSocket->AttachActor(this, inOwnerCharacter->GetMesh()); // 把本武器绑定在player的mesh上

			ABasePlayer* ownerPlayer = Cast<ABasePlayer>(inOwnerCharacter);
			if (ownerPlayer) {
				ownerPlayer->SetWeaponHold(this, bEquipOnRightHand);
				bOwnerIsPlayer = true;
			}
			ABaseEnemy* ownerEnemy = Cast<ABaseEnemy>(inOwnerCharacter);
			if (ownerEnemy) {
				ownerEnemy->SetWeaponHold(this, bEquipOnRightHand);
				bOwnerIsPlayer = false;
			}
			ownerCharacter = inOwnerCharacter;
		}

	}

}
