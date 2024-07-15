// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
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

	TMap<EMonsterAnim, class UAnimMontage*> GetAnimMontage() const
	{
		return AnimMontages;
	}

private:
	/// <summary>
	/// �⺻ Monster ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TSubclassOf<AActor> MonsterUClass = TSubclassOf<AActor>();

	/// <summary>
	/// �ȱ� �ӵ�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float WalkSpeed = 300.0f;

	/// <summary>
	/// �޸��� �ӵ�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float RunSpeed = 600.0f;

	/// <summary>
	/// ��Ÿ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TMap<EMonsterAnim, class UAnimMontage*> AnimMontages;
};

UCLASS()
class UMonsterData : public UObject
{
	GENERATED_BODY()

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	const FMonsterDataRow* Data;
	
	FVector OriginPos = FVector::ZeroVector;
	UPROPERTY(Replicated)
	float Hp = 100.0f;

	float IdleTime = 0.0f;
	
	float Max_PatrolRange = 800.0f;
	
	float AttackTime = 0.0f;
	float AttackDamage = 0.0f;
	float AttackBoundary = 200.0f;

	float ClimbTime = 0.0f;
	
};
