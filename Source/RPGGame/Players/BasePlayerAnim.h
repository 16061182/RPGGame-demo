// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BasePlayerAnim.generated.h"

class ABasePlayer;

UCLASS()
class RPGGAME_API UBasePlayerAnim : public UAnimInstance {
	GENERATED_BODY()
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeedForward;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float moveSpeedRight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	APawn* pawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	ABasePlayer* ownerBasePlayer;

protected:
	virtual void NativeInitializeAnimation() override; // 执行初始化工作，注意不写在构造函数里

public:
	//virtual void NativeUpdateAnimation(float DeltaSeconds) override; // 每帧刷新
	UFUNCTION(BlueprintCallable, Category = AnimProperty)
	void UpdateAnimProperties();
};
