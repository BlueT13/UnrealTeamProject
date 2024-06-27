// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
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


protected:

private:
	/// <summary>
	/// �⺻ Monster ����
	/// </summary>
	UPROPERTY(Category = "Parameter", EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TSubclassOf<AActor> MonsterUClass = TSubclassOf<AActor>();

	/// <summary>
	/// �ȱ� �ӵ�
	/// </summary>
	UPROPERTY(Category = "Parameter", EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float WalkSpeed = 300.0f;

	/// <summary>
	/// �޸��� �ӵ�
	/// </summary>
	UPROPERTY(Category = "Parameter", EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float RunSpeed = 600.0f;

};

UCLASS()
class UMonsterData : public UObject
{
	GENERATED_BODY()

public:
	const FMonsterDataRow* Data;

	float IdleTime = 0.0f;
};
