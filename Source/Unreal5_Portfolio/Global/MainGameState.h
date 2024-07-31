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
	/// ���� ���� ���� ���� üũ �Լ�
	/// </summary>
	UFUNCTION(Reliable, NetMulticast)
	void GameStateCheck(AActor* _OtherActor);
	void GameStateCheck_Implementation(AActor* _OtherActor);

	/// <summary>
	/// ����(AActor)�� ��� ���� ������ GameState ���� ����
	/// </summary>
	/// <param name="_OtherActor"></param>
	UFUNCTION(Reliable, NetMulticast)
	void GameStateModify(AActor* _OtherActor);
	void GameStateModify_Implementation(AActor* _OtherActor);

	/// <summary>
	/// GameState ���� ���� ���� �Լ�
	/// </summary>
	/// <param name="_OtherActor"></param>
	UFUNCTION()
	void GameStateConditionUpdate(AActor* _OtherActor, bool _IsAdd);

	UFUNCTION()
	FORCEINLINE int GetPlayerCount()
	{
		return PlayerCount;
	}

	UFUNCTION()
	FORCEINLINE void AddPlayerCount()
	{
		++PlayerCount;
	}

	UFUNCTION()
	FORCEINLINE void SubPlayerCount()
	{
		if (0 >= PlayerCount)
		{
			PlayerCount = 0;
			return;
		}

		--PlayerCount;
	}

	UFUNCTION()
	FORCEINLINE bool GetIsStageChange() const
	{
		return IsStageChange;
	}

	UFUNCTION()
	FORCEINLINE void SetIsStageChange(bool _Value)
	{
		IsStageChange = _Value;
	}

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
	/// ���� Stage�� �ٲ�������� ���� ���� ����
	/// </summary>
	UPROPERTY()
	bool IsStageChange = false;

	/// <summary>
	/// ���� Ȯ���� Bomb ��
	/// </summary>
	UPROPERTY()
	int BombCount = 0;

	/// <summary>
	/// Ȯ�� ������ �ִ� Bomb ��
	/// </summary>
	UPROPERTY()
	int MaxBombCount = 1;

	/// <summary>
	/// �÷��̾� ��
	/// </summary>
	UPROPERTY()
	int PlayerCount = 0;

	/// <summary>
	/// �÷��̾� �ִ� ��
	/// </summary>
	UPROPERTY()
	int MaxPlayerCount = 2;

	/// <summary>
	/// üũ�� ������ ī��Ʈ ��
	/// </summary>
	UPROPERTY()
	int ItemCount = 0;
};
