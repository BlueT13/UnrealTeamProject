// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "DataTable/NetDataRow.h"
#include "MainGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class UNREAL5_PORTFOLIO_API UMainGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	void Init() override;

	/// <summary>
	/// PlayerDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ PlayerDataTable ���̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const struct FPlayerDataRow* GetPlayerData(FName _Name);

	/// <summary>
	/// NetDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ NetDataTable ���̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const struct FNetDataRow* GetNetData(FName _Name);

	/// <summary>
	/// MonsterDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ MonsterDataTable ���̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const struct FMonsterDataRow* GetMonsterData(FName _Name);

	/// <summary>
	/// MonsterDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ MonsterDataTable ���̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const struct FMonsterSpawnerDataRow* GetMonsterSpawnerData(FName _Name);

	/// <summary>
	/// ItemDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ ItemDataTable �� �̸�
	/// </summary>
	const struct FItemDataRow* GetItemData(FName _Name);

	/// <summary>
	/// InGameUserWidgetDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ InGameUserWidgetDataTable �� �̸�
	/// </summary>
	const struct FInGameUserWidgetDataRow* GetInGameUserWidgetDataTable(FName _Name);

	/// <summary>
	/// BossDataTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ BossDataTable �� �̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const struct FBossDataRow* GetBossDataTable(FName _Name);

	/// <summary>
	/// GlobalObjectTable �����͸� �޾ƿ��� �Լ�
	/// FName�� ������ GlobalObjectTable �� �̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	const TSubclassOf<UObject> GetGlobalObjectClass(FName _Name);

	/// <summary>
	/// TestInGameWidgets �� �޾ƿ��� �Լ�
	/// </summary>
	TMap<FString, TSubclassOf<UUserWidget>>& GetInGameWidgets()
	{
		return TestInGameWidgets;
	}

	/// <summary>
	/// MapObjDataTable �����͸� ��ȯ
	/// FName�� ������ MapObjDataTable �� �̸�
	/// </summary>
	/// <param name="_Name"></param>
	/// <returns></returns>
	struct FMapObjDataRow* GetMapObjDataTable(FName _Name);

	FNetDataRow CurNetInfo;
	FRandomStream Random;

	UFUNCTION()
	FORCEINLINE int GetNetToken()
	{
		return ++NetToken;
	}

protected:

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* PlayerDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* NetDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* MonsterDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* MonsterSpawnerDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* ItemDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* InGameUserWidgetDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* BossDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* GlobalObjectTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowprivateAccess = "true"))
	UDataTable* MapObjDataTable = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<FString, TSubclassOf<UUserWidget>> TestInGameWidgets;
private:
	UMainGameInstance();

	UPROPERTY()
	int NetToken = -1;
};
