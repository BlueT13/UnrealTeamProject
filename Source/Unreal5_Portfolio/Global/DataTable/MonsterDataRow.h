// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "Global/Animation/MainMultiAnimInstance.h"
#include "MonsterDataRow.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	TSubclassOf<AActor> GetMonsterUClass() const
	{
		return MonsterUClass;
	}

	float GetWalkSpeed() const
	{
		return WalkSpeed;
	}

	float GetRunSpeed() const
	{
		return RunSpeed;
	}

	float GetAttackRange() const
	{
		return AttackRange;
	}

	TMap<EBasicMonsterAnim, FAnimMontageGroup> GetAllAnimMontage() const
	{
		return AllAnimMontages;
	}

	// Test
	ETestMonsterType GetMonsterType() const
	{
		return MonsterType;
	}

	TMap<ETestMonsterAnim, FAnimMontageGroup> GetTestAnimMontage() const
	{
		return TestAnimMontages;
	}

private:
	// ���� UClass
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	TSubclassOf<AActor> MonsterUClass = TSubclassOf<AActor>();

	// �ȱ� �ӵ�
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float WalkSpeed = 300.0f;

	// �޸��� �ӵ�
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float RunSpeed = 600.0f;

	// ���� ����
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	float AttackRange = 200.0f;

	// ��Ÿ��
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	TMap<EBasicMonsterAnim, FAnimMontageGroup> AllAnimMontages;
	
	// Test
	// ���� Ÿ��
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	ETestMonsterType MonsterType = ETestMonsterType::Noraml;

	// ��Ÿ��
	UPROPERTY(EditAnywhere, meta = (AllowprivateAccess = "true"))
	TMap<ETestMonsterAnim, FAnimMontageGroup> TestAnimMontages;

};