// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "BossDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FBossDataRow : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	FORCEINLINE FString GetName() const
	{
		return Name;
	}

	FORCEINLINE float GetHP() const
	{
		return HP;
	}

	FORCEINLINE float GetMeleeAttackDamage() const
	{
		return MeleeAttackDamage;
	}

	FORCEINLINE float GetRangedAttackDamage() const
	{
		return RangedAttackDamage;
	}

	FORCEINLINE float GetAttackTime() const
	{
		return AttackTime;
	}

	FORCEINLINE float GetMeleeAttackBoundary() const
	{
		return MeleeAttackBoundary;
	}

	FORCEINLINE float GetRangedAttackBoundary() const
	{
		return RangedAttackBoundary;
	}

	FORCEINLINE float GetWalkSpeed() const
	{
		return WalkSpeed;
	}

	FORCEINLINE float GetRunSpeed() const
	{
		return RunSpeed;
	}

	TMap<EBossMonsterAnim, class UAnimMontage*> GetAnimMontage() const
	{
		return AnimMontages;
	}

protected:

private:
	/// <summary>
	/// Boss �̸�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	FString Name = "None";

	/// <summary>
	/// ü��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float HP = 0.0f;

	/// <summary>
	/// ���� ���ݷ�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float MeleeAttackDamage = 0.0f;

	/// <summary>
	/// ���Ÿ� ���ݷ�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float RangedAttackDamage = 0.0f;

	/// <summary>
	/// ���� ��Ÿ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float AttackTime = 0.0f;

	/// <summary>
	/// ���� ���� ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float MeleeAttackBoundary = 0.0f;

	/// <summary>
	/// ���Ÿ� ���� ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float RangedAttackBoundary = 0.0f;

	/// <summary>
	/// ���� �ȱ� �ӵ�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float WalkSpeed = 0.0f;

	/// <summary>
	/// ���� �޸��� Speed
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float RunSpeed = 0.0f;

	/// <summary>
	/// ���� ��Ÿ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TMap<EBossMonsterAnim, class UAnimMontage*> AnimMontages;
};

UCLASS()
class UBossData : public UObject
{
	GENERATED_BODY()

protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	const FBossDataRow* Data = nullptr;

	float HP = 0.0f;

protected:

private:

};