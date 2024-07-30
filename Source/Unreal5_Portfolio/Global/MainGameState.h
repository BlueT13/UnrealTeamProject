// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Global/ContentsEnum.h"
#include "MainGameState.generated.h"

UCLASS()
class UNREAL5_PORTFOLIO_API UActorGroup : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> Actors;
};

/**
 * 
 */
UCLASS()
class UNREAL5_PORTFOLIO_API AMainGameState : public AGameState
{
	GENERATED_BODY()

public:
	template<typename EnumType>
	void PushActor(EnumType _Index, AActor* _Actor)
	{
		PushActor(static_cast<uint8>(_Index), _Actor);
	}

	void PushActor(uint8 _Index, AActor* _Actor);

	template<typename EnumType>
	UActorGroup* GetActorGroup(EnumType _Index)
	{
		return GetActorGroup(static_cast<uint8>(_Index));
	}

	UActorGroup* GetActorGroup(uint8 _Index);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGameStage GetCurStage() const
	{
		return CurStage;
	}

	UFUNCTION()
	FORCEINLINE void SetCurStage(EGameStage _Stage)
	{
		CurStage = _Stage;
	}

	/// <summary>
	/// Melee Ȯ���� ȣ��Ǵ� �Լ�
	/// </summary>
	UFUNCTION(Reliable, NetMulticast)
	void AddMeleeNum();
	void AddMeleeNum_Implementation();

	/// <summary>
	/// Ȯ���� Melee ���� ��ȯ
	/// </summary>
	/// <returns></returns>
	FORCEINLINE int GetMeleeNum() const
	{
		return MeleeNum;
	}

	/// <summary>
	/// ������� ������ Ȯ���� ������ ���� �����ϴ� �Լ�
	/// </summary>
	void AddArmoryWeaponNum(EPlayerPosture _ItemType);

	/// <summary>
	/// Ȯ���� Rifle ���� ��ȯ
	/// </summary>
	/// <returns></returns>
	FORCEINLINE int GetRifleNum() const
	{
		return RifleNum;
	}

	/// <summary>
	/// Ȯ���� Bomb ���� ��ȯ
	/// </summary>
	/// <returns></returns>
	FORCEINLINE int GetBombNum() const
	{
		return BombNum;
	}

	/// <summary>
	/// ����1 �������� Ŭ����
	/// </summary>
	void SetClearBoss1Stage();
	
	/// <summary>
	/// ����2 �������� Ŭ����
	/// </summary>
	void SetClearBoss2Stage();

protected:
	AMainGameState();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<uint8, UActorGroup*> AllActor;
	
	/// <summary>
	/// ���� ���� Stage ����(�ܰ�)
	/// </summary>
	UPROPERTY(Replicated)
	EGameStage CurStage = EGameStage::Init;

	/// <summary>
	/// ���� Ȯ���� Melee ��
	/// </summary>
	UPROPERTY(Replicated)
	int MeleeNum = 0;

	/// <summary>
	/// Ȯ�� ������ �ִ� Melee ��
	/// </summary>
	UPROPERTY()
	int MaxMeleeNum = 4;

	/// <summary>
	/// ���� Ȯ���� Rifle ��
	/// </summary>
	UPROPERTY(Replicated)
	int RifleNum = 0;

	/// <summary>
	/// Ȯ�� ������ �ִ� Rifle ��
	/// </summary>
	UPROPERTY()
	int MaxRifleNum = 4;

	/// <summary>
	/// ���� Ȯ���� Bomb ��
	/// </summary>
	UPROPERTY(Replicated)
	int BombNum = 0;

	/// <summary>
	/// Ȯ�� ������ �ִ� Bomb ��
	/// </summary>
	UPROPERTY()
	int MaxBombNum = 1;

	/// <summary>
	/// Boss1Stage Ŭ���� ����
	/// </summary>
	UPROPERTY(Replicated)
	bool ClearBoss1Stage = false;

	/// <summary>
	/// Boss2Stage Ŭ���� ����
	/// </summary>
	UPROPERTY(Replicated)
	bool ClearBoss2Stage = false;

};
