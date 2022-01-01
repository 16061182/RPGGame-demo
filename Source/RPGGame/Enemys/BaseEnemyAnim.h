// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseEnemyAnim.generated.h"

class ABaseEnemy;

UCLASS()
class RPGGAME_API UBaseEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeedForward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeedRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	APawn* pawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	ABaseEnemy* ownerBaseEnemy;

protected:
	virtual void NativeInitializeAnimation() override;

public:
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	UFUNCTION(BlueprintCallable, Category = AnimProperty)
	void UpdateAnimProperties();
	
};
