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
	UFUNCTION()
	void AddMeleeNum()
	{
		if (MaxMeleeNum <= MeleeNum)
		{
			return;
		}

		++MeleeNum;

		if (MaxMeleeNum == MeleeNum)
		{
			CurStage = EGameStage::VisitArmory;
			return;
		}
	}

	/// <summary>
	/// Ȯ���� Melee ���� ��ȯ
	/// </summary>
	/// <returns></returns>
	UFUNCTION()
	FORCEINLINE int GetMeleeNum() const
	{
		return MeleeNum;
	}

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TMap<uint8, UActorGroup*> AllActor;
	
	/// <summary>
	/// ���� ���� Stage ����(�ܰ�)
	/// </summary>
	UPROPERTY()
	EGameStage CurStage = EGameStage::Init;


	/// <summary>
	/// ���� Ȯ���� Melee ��
	/// </summary>
	UPROPERTY()
	int MeleeNum = 0;

	/// <summary>
	/// Ȯ�� ������ �ִ� Melee ��
	/// </summary>
	UPROPERTY()
	int MaxMeleeNum = 4;

};
