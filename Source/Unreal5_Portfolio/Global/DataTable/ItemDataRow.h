// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Global/ContentsEnum.h"
#include "ItemDataRow.generated.h"


USTRUCT(BlueprintType)
struct FItemDataRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	/// <summary>
	/// ������ Ÿ�� ��ȯ
	/// </summary>
	/// <returns></returns>
	EPlayerPosture GetType() const
	{
		return Type;
	}

	/// <summary>
	/// ������ �̸� ��ȯ
	/// </summary>
	/// <returns></returns>
	FString GetName() const
	{
		return Name;
	}

	/// <summary>
	/// ������ ������ ��ȯ
	/// </summary>
	/// <returns></returns>
	int GetDamage() const
	{
		return Damage;
	}

	/// <summary>
	/// ���� ���� ��ȯ
	/// </summary>
	/// <returns></returns>
	int GetReloadNum() const
	{
		return ReloadNum;
	}

	/// <summary>
	/// ������ StaticMesh ������ ��ȯ
	/// </summary>
	/// <returns></returns>
	class UStaticMesh* GetResMesh() const
	{
		return ResMesh;
	}

	/// <summary>
	/// �������� UClass ��ȯ.
	/// </summary>
	/// <returns></returns>
	TSubclassOf<AActor> GetItemUClass() const
	{
		return ItemUClass;
	}

	/// <summary>
	/// ������ static mesh component�� relative location �� ��ȯ
	/// </summary>
	/// <returns></returns>
	FORCEINLINE FVector GetRelLoc() const
	{
		return RelLoc;
	}

	/// <summary>
	/// ������ static mesh component�� relative rotation �� ��ȯ
	/// </summary>
	/// <returns></returns>
	FORCEINLINE FRotator GetRelRot() const
	{
		return RelRot;
	}

protected:

private:
	/// <summary>
	/// ������ ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	EPlayerPosture Type = EPlayerPosture::Barehand;

	/// <summary>
	/// ������ �̸�
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	FString Name = "";

	/// <summary>
	/// ������ ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	int Damage = 0;

	/// <summary>
	/// ���� ����
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	int ReloadNum = -1;

	/// <summary>
	/// ������ StaticMesh ���ҽ� ���� ��
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	class UStaticMesh* ResMesh = nullptr;

	/// <summary>
	/// �������� UClass
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	TSubclassOf<AActor> ItemUClass = TSubclassOf<AActor>();

	/// <summary>
	/// ������ static mesh component�� relative location
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	FVector RelLoc = FVector(0.0f, 0.0f, 0.0f);

	/// <summary>
	/// ������ static mesh component�� relative rotation
	/// </summary>
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	FRotator RelRot = FRotator(0.0f, 0.0f, 0.0f);
};
