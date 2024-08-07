// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "BossMonsterDataRow.generated.h"

class UAnimMontage;

USTRUCT()
struct UNREAL5_PORTFOLIO_API FRangedAttackRange
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float Min = 0.0f;

	UPROPERTY(EditAnywhere)
	float Max = 0.0f;
};


USTRUCT(BlueprintType)
struct FBossMonsterDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// ���� �̸�
	UPROPERTY(EditAnywhere)
	FString BossName;

	// �ִ� ü��
	UPROPERTY(EditAnywhere)
	float MaxHp = 0.0f;

	// ���� ������
	UPROPERTY(EditAnywhere)
	float MeleeAttackDamage = 20.0f;

	// ���� ���� ����
	UPROPERTY(EditAnywhere)
	float MeleeAttackRange = 0.0f;

	// ���Ÿ� ���� ���� 
	UPROPERTY(EditAnywhere)
	FRangedAttackRange RangedAttackRange;

	// �߰� �ӵ�
	UPROPERTY(EditAnywhere)
	float ChaseSpeed = 0.0f;

	// ��Ÿ��
	UPROPERTY(EditAnywhere)
	TMap<EBossMonsterAnim, class UAnimMontage*> AnimMontages;
};
