// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "BossMonsterDataRow.generated.h"

class UAnimMontage;

/**
 * 
 */
USTRUCT(BlueprintType)

struct FBossMonsterDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	// ���� ������
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float MeleeAttackDamage = 20.0f;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float MeleeAttackRange = 200.0f;

	// ����ü ���ݷ�
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float ProjectileDamage = 10.0f;

	// ���Ÿ� ���� ����
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float RangedAttackRange = 1000.0f;

	// ���� ��Ÿ��
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float AttackCoolTime = 0.0f;

	// �ȱ� �ӵ�
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float WalkSpeed = 50.0f;

	// �߰� �ӵ�
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float ChaseSpeed = 200.0f;

	// ��Ÿ��
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	TMap<EBossMonsterAnim, class UAnimMontage*> AnimMontages;

};
