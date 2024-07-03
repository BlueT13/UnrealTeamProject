// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MonsterSpawnerDataRow.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FMonsterSpawnerDataRow : public FTableRowBase
{
	GENERATED_BODY()

private:
	/// <summary>
	/// Spawner ��ġ
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TArray<FVector> Location;

	/// <summary>
	/// ���� �ּ� ���� ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TArray<int> Min_SpawnNum;

	/// <summary>
	/// ���� �ִ� ���� ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TArray<int> Max_SpawnNum;

};