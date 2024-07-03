// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "MonsterSpawnerDataRow.generated.h"

USTRUCT(BlueprintType)
struct FMonsterSpawnerInfo : public FTableRowBase
{
	GENERATED_BODY()

	// ������ Ÿ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	EMonsterSpawnerType Type;

	// ������ ��ġ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	FVector Location;

	// ���� �ݰ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	float Ridus;

	// �ּ� ���� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	int MinSpawnNum;

	// �ִ� ���� ��
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	int MaxSpawnNum;

};

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterSpawnerDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	TArray<FMonsterSpawnerInfo> GetSpawnerInfo() const
	{
		return SpawnerInfo;
	}

private:
	/// <summary>
	/// ���� Spawn ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TArray<FMonsterSpawnerInfo> SpawnerInfo;

};

UCLASS()
class UMonsterSpawnerData : public UObject
{
	GENERATED_BODY()

public:
	EMonsterSpawnerType Type = EMonsterSpawnerType::Max;
	FVector Location = FVector::ZeroVector;
	float Radius = 0.0f;
	int MinSize = -1;
	int MaxSize = -1;

};