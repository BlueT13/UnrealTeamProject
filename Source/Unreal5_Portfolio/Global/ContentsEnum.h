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

UENUM(BlueprintType)
enum class EGameStage : uint8
{
	Init					UMETA(DisplayName = "���� �÷��� ���� �ʱ�"),
	VisitArmory				UMETA(DisplayName = "����� �湮"),
	ObtainFirstSample		UMETA(DisplayName = "����1 ������ Ȯ��"),
	ObtainSecondSample		UMETA(DisplayName = "����2 ������ Ȯ��"),
	ObtainThirdSample		UMETA(DisplayName = "����3 ������ Ȯ��"),
	PlantingBomb			UMETA(DisplayName = "��ź ��ġ"),
	MoveToGatheringPoint	UMETA(DisplayName = "�������� �̵�"),
	Defensing				UMETA(DisplayName = "���� ���"),
	MissionClear			UMETA(DisplayName = "�̼� ����")
};


/// <summary>
/// 1, 2, 3, 4 ���� ���� �÷��̾� �ڼ�.
/// </summary>
UENUM(BlueprintType)
enum class EPlayerPosture : uint8
{
	Rifle1,			// �� ���� 1		// �ش� ������� �ڼ� ����
	Rifle2,			// �� ���� 2		// �ش� ������� �ڼ� ����
	Melee,			// �ٰŸ� ����	// �ش� ������� �ڼ� ����
	Drink,			// ����(����)	// �κ��丮���� ����
	Bomb,			// ��ġ�� ��ź	// �κ��丮���� ����
	Barehand,		// �Ǽ�			// �ش� ������� �ڼ� ����
	SlotMax
};

UENUM(BlueprintType)
enum class ETestMonsterState : uint8
{
	Idle,
	Patrol,
	Chase,
	Fall,
	Jump,
	Climb,
	ClimbEnd,
	Attack,
	Dead,
	JumpAttack,
	Scream
};

UENUM(BlueprintType)
enum class ETestMonsterType : uint8
{
	Noraml,
	Crawl,
	Boss
};

UENUM(BlueprintType)
enum class EBasicMonsterState : uint8
{
	Idle,
	Patrol,
	Chase,
	Fall,
	Jump,
	Climb,
	ClimbEnd,
	Attack,
	Dead
};

UENUM(BlueprintType)
enum class ETestMonsterAnim : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Run			UMETA(DisplayName = "Run"),
	Attack		UMETA(DisplayName = "Attack"),
	Dead		UMETA(DisplayName = "Dead"),
	Climb		UMETA(DisplayName = "Climb"),
	ClimbEnd	UMETA(DisplayName = "ClimbEnd"),
	Scream		UMETA(DisplayName = "Scream"),
	JumpAttack	UMETA(DisplayName = "JumpAttack"),
};

UENUM(BlueprintType)
enum class EBasicMonsterAnim : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Walk		UMETA(DisplayName = "Walk"),
	Run			UMETA(DisplayName = "Run"),
	Attack		UMETA(DisplayName = "Attack"),
	Dead		UMETA(DisplayName = "Dead"),
	Climb		UMETA(DisplayName = "Climb"),
	ClimbEnd	UMETA(DisplayName = "ClimbEnd")
};

UENUM()
enum class EMonsterSpawnerType : uint8
{
	Once,
	Continuous,
	Max
};

UENUM(BlueprintType)
enum class EBossMonsterState : uint8
{
	Idle,
	Chase,
	MeleeAttack,
	RangedAttack,
	Dead
};

UENUM(BlueprintType)
enum class EBossMonsterAnim : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	WalK	UMETA(DisplayName = "Walk"),
	Run	UMETA(DisplayName = "Run"),
	MeleeAttack	UMETA(DisplayName = "MeleeAttack"),
	RangedAttack	UMETA(DisplayName = "RangedAttack"),
	Dead	UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class EUserWidgetType : uint8
{
	// InGame
	HpBar,
	WeaponSlot,
	MiniMap,
	Leaderboard,
	Crosshair,
	WorldMap,
	ReloadComment,

	// Title
	TitleBackground,
	TitleLogo,
	ServerBtn,
	ConnectBrowser,
	SettingBrowser,
};


UENUM(BlueprintType)
enum class EPlayerLowerState : uint8
{
	Idle	UMETA(DisplayName = "Idle"),
	Crouch	UMETA(DisplayName = "CrouchIdle"),
};

UENUM(BlueprintType)
enum class EPlayerUpperState : uint8
{
	Barehand_Attack	UMETA(DisplayName = "Barehand_Attack"),		// �Ǽ� ����
	Rifle_Attack		UMETA(DisplayName = "Rifle_Attack"),	// �ֹ��� ����
	//Pistol		UMETA(DisplayName = "PistolAttack"),		// ��������
	//Melee		UMETA(DisplayName = "MeleeAttack"),				// �ٰŸ�����
	//Throwing	UMETA(DisplayName = "ThrowingAttack"),			// ��ô����
	//Supply		UMETA(DisplayName = "Supplies"),			// �� �� �Ҹ�ǰ ������
	//SlotMax
};

UENUM(BlueprintType)
enum class EPlayerMoveDir : uint8
{
	Forward	UMETA(DisplayName = "Forward"),
	Back	UMETA(DisplayName = "Back"),
	Left	UMETA(DisplayName = "Left"),
	Right	UMETA(DisplayName = "Right"),
};

UCLASS()
class UNREAL5_PORTFOLIO_API UContentsEnum : public UObject
{
	GENERATED_BODY()
	
};
