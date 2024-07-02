// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ContentsEnum.generated.h"


// Contents ���������� ����ϴ� Enum class
UENUM(BlueprintType)
enum class EObjectType : uint8
{
	Player,
	Monster,
	Max
};

UENUM(BlueprintType)
enum class EPlayerState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Walk	UMETA(DisplayName = "Walk"),
	Run		UMETA(DisplayName = "Run"),
	Jump	UMETA(DisplayName = "Jump"),
	Crouch	UMETA(DisplayName = "Crouch"),
	Fire	UMETA(DisplayName = "Fire"),
};

/// <summary>
/// 1, 2, 3, 4 ���� ���� �÷��̾� �ڼ�.
/// </summary>
UENUM(BlueprintType)
enum class EPlayerPosture : uint8
{
	Barehand	UMETA(DisplayName = "BarehandAttack"),	// �Ǽհ���
	Rifle		UMETA(DisplayName = "RifleAttack"),		// �ֹ���(Rifle)
	Pistol		UMETA(DisplayName = "PistolAttack"),	// ��������(Pistol)
	Melee		UMETA(DisplayName = "MeleeAttack"),		// �ٰŸ�����
	Throwing	UMETA(DisplayName = "ThrowingAttack"),	// ��ô����
	Item		UMETA(DisplayName = "Item")				// �� �� ������
};

UENUM(BlueprintType)
enum class EMonsterState : uint8
{
	Idle,
	Patrol,
	Chase,
	Attack,
	Dead
};

UENUM(BlueprintType)
enum class EMonsterAnim : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Walk	UMETA(DisplayName = "Walk"),
	Run		UMETA(DisplayName = "Run"),
	Attack	UMETA(DisplayName = "Attack"),
	Dead	UMETA(DisplayName = "Dead")
};

UCLASS()
class UNREAL5_PORTFOLIO_API UContentsEnum : public UObject
{
	GENERATED_BODY()
	
};
