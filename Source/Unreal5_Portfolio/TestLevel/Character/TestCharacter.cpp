// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/ContentsLog.h"
#include "Global/DataTable/ItemDataRow.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevel/Character/TestPlayerState.h"
#include "TestLevel/UI/TestMinimapIconComponent.h"
#include "TestLevel/UI/TestPlayHUD.h"
#include "TestLevel/UI/TestHpBarUserWidget.h"
#include "TestLevel/Item/WeaponComponent.h"
#include "TestLevel/Item/WeaponBase.h"
#include "PartDevLevel/Character/PlayerAnimInstance.h"
#include "PartDevLevel/Monster/TestMonsterBase.h"
#include "PartDevLevel/Monster/Boss/TestBossMonsterBase.h"
#include "MainGameLevel/Object/MapObjectBase.h"
#include "MainGameLevel/Object/Bomb.h"
#include "MainGameLevel/UI/InGame/HeadNameWidgetComponent.h"
#include "TestPlayerController.h"


// Sets default values
ATestCharacter::ATestCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Character Mesh => ����ĳ���� ���� �ʿ� (24.07.29 ������) => ���� ����.
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	GetMesh()->bHiddenInSceneCapture = true;

	// Item Mesh => ����ĳ���� ����.
	ItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemSocketMesh"));
	ItemSocketMesh->SetupAttachment(GetMesh(), FName("ItemSocket"));
	ItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSocketMesh->SetGenerateOverlapEvents(true);
	ItemSocketMesh->SetOwnerNoSee(true);
	ItemSocketMesh->SetVisibility(false);
	ItemSocketMesh->SetIsReplicated(true);
	ItemSocketMesh->bHiddenInSceneCapture = true;

	// SpringArm Component => ����ĳ���� ���� �ʿ� (24.07.29 ������)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc);
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = true;

	// Camera Component => ����ĳ���� ���� �ʿ� (24.07.29 ������)
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);

	// FPV Character Mesh => ����ĳ���� ���� �ʿ� (24.07.29 ������)
	FPVMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPVMesh->SetupAttachment(CameraComponent);
	FPVMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -160.0f));
	FPVMesh->SetOwnerNoSee(false);
	FPVMesh->SetOnlyOwnerSee(true);
	FPVMesh->bCastDynamicShadow = false;
	FPVMesh->CastShadow = false;

	// FPV Item Mesh => ����ĳ���� ����.
	FPVItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPVItemSocketMesh"));
	FPVItemSocketMesh->SetupAttachment(FPVMesh, FName("FPVItemSocket"));
	FPVItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	FPVItemSocketMesh->SetGenerateOverlapEvents(true);
	FPVItemSocketMesh->SetOnlyOwnerSee(true);
	FPVItemSocketMesh->SetVisibility(false);
	FPVItemSocketMesh->SetIsReplicated(true);
	FPVItemSocketMesh->bCastDynamicShadow = false;
	FPVItemSocketMesh->CastShadow = false;

	// Map Item �˻� => ����ĳ���� ����.
	GetMapItemCollisonComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("GetMapItemCollisionComponent"));
	GetMapItemCollisonComponent->SetupAttachment(RootComponent);
	GetMapItemCollisonComponent->SetRelativeLocation(FVector(100.0, 0.0, -20.0f));
	GetMapItemCollisonComponent->SetCollisionProfileName(FName("MapItemSearch"));

	UEnum* Enum = StaticEnum<EPlayerPosture>();

	// = > ����ĳ���� ����. [�ּ� �κ� �ٸ��� Ȯ�� ��û.]
	for (size_t i = 0; i < static_cast<size_t>(EPlayerPosture::Barehand); i++)
	{
		// Inventory (for UI Test)
		FItemInformation NewSlot;
		/*NewSlot.Name = "";
		NewSlot.ReloadMaxNum = -1;
		NewSlot.ReloadLeftNum = -1;*/
		ItemSlot.Push(NewSlot);
		IsItemIn.Push(false);
	}

	// HandAttack Component => ����ĳ���� ����.[�ּ��� ���� 3�� ����. Ȯ�� �ʿ�.]
	//FString Name = "Punch";
	HandAttackComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Hand Attack Comp"));
	HandAttackComponent->SetupAttachment(GetMesh());
	HandAttackComponent->SetRelativeLocation({ 0.0f, 80.0f, 120.0f });

	// MinimapIcon Component => ����ĳ���� ����.
	MinimapIconComponent = CreateDefaultSubobject<UTestMinimapIconComponent>(TEXT("MinimapPlayerIcon"));
	MinimapIconComponent->SetupAttachment(RootComponent);
	MinimapIconComponent->bVisibleInSceneCaptureOnly = true;

	// HeadName Component	// => �������� ���� �ʿ� (24.07.30 �߰���)
	HeadNameComponent = CreateDefaultSubobject<UHeadNameWidgetComponent>(TEXT("HeadNameWidgetComponent"));
	HeadNameComponent->SetupAttachment(RootComponent);
	HeadNameComponent->bHiddenInSceneCapture = true;

	// Riding Character Mesh => ����ĳ���� ����.(�ּ�)
	RidingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RidingMesh"));
	RidingMesh->SetupAttachment(GetMesh());
	RidingMesh->SetCollisionProfileName(TEXT("NoCollision"));
	RidingMesh->SetVisibility(false);
	RidingMesh->SetIsReplicated(true);
	RidingMesh->bHiddenInSceneCapture = true;
}

//void ATestCharacter::Collision(AActor* _OtherActor, UPrimitiveComponent* _Collision)
//{
//	ATestMonsterBase* Monster = Cast<ATestMonsterBase>(_OtherActor);
//	if (nullptr == Monster)
//	{
//		return;
//	}
//}

void ATestCharacter::HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision) // => ���� ĳ���Ϳ� ����.
{
	{
		ATestMonsterBase* Monster = Cast<ATestMonsterBase>(_OtherActor);
		if (nullptr != Monster)
		{
			Monster->Damaged(150.0f);
		}
	}

	{
		ATestBossMonsterBase* BossMonster = Cast<ATestBossMonsterBase>(_OtherActor);
		if (nullptr != BossMonster)
		{
			BossMonster->Damaged(150.0f);
		}
	}
}

void ATestCharacter::ChangeHandAttackCollisionProfile(FName _Name) // => ���� ����.
{
	HandAttackComponent->SetCollisionProfileName(_Name);
}

void ATestCharacter::GetDamage(float _Damage)
{
	PlayerHp -= _Damage;
}

// ���� �÷��̾� �߰� �ʿ� �ڵ� (��ȯ) 07/24 => ���� ����.
void ATestCharacter::PostInitializeComponents()
{
	if (GetWorld()->WorldType == EWorldType::Game
		|| GetWorld()->WorldType == EWorldType::PIE)
	{
		UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
		if (nullptr == MainGameInst)
		{
			return;
		}

		// ���̷�Ż �޽� ����
		USkeletalMesh* PlayerSkeletalMesh = MainGameInst->GetPlayerData(FName("TestPlayer"))->GetPlayerSkeletalMesh();
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh);

		// ABP ����
		UClass* AnimInst = Cast<UClass>(MainGameInst->GetPlayerData(FName("TestPlayer"))->GetPlayerAnimInstance());
		GetMesh()->SetAnimInstanceClass(AnimInst);
	}

	Super::PostInitializeComponents();

}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()
{
	NetCheck();
	Super::BeginPlay();

	UMainGameBlueprintFunctionLibrary::PushActor(EObjectType::Player, this);

	// ��Ÿ�� ���濡 �ʿ��� ���� �߰� �ʿ� (��ȯ) // => ���� ����.
	PlayerAnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	FPVPlayerAnimInst = Cast<UPlayerAnimInstance>(FPVMesh->GetAnimInstance());

	HandAttackComponent->SetCollisionProfileName(TEXT("NoCollision"));
	//UISetting();

	SettingPlayerState();
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerHp(DeltaTime); // => ���� ����.

	// 7/29 ���� ������ �� ĳ���� ȸ�� ���� �ڵ�
	if (IsFaint == false) // ���� ���� 
	{
		bUseControllerRotationYaw = true;
	}
	else // ���� ����
	{
		bUseControllerRotationYaw = false;
	}

#if WITH_EDITOR
	// GameState ���� ��¿� TestCode
	{
		AMainGameState* CurGameState = UMainGameBlueprintFunctionLibrary::GetMainGameState(GetWorld());

		if (nullptr == CurGameState)
		{
			return;
		}

		int MeleeNum = CurGameState->GetMeleeNum();
		FString MNumString = FString::FromInt(MeleeNum);
		UMainGameBlueprintFunctionLibrary::DebugTextPrint(GetWorld(), FString(TEXT("CurMeleeNum = ")) + MNumString);

		int RifleNum = CurGameState->GetRifleNum();
		FString RNumString = FString::FromInt(RifleNum);
		UMainGameBlueprintFunctionLibrary::DebugTextPrint(GetWorld(), FString(TEXT("CurRifleNum = ")) + RNumString);

		int BombNum = CurGameState->GetBombNum();
		FString BNumString = FString::FromInt(BombNum);
		UMainGameBlueprintFunctionLibrary::DebugTextPrint(GetWorld(), FString(TEXT("CurBombNum = ")) + BNumString);


		EGameStage StageNum = CurGameState->GetCurStage();
		FString StageString = FString();
		const UEnum* StateEnum = FindObject<UEnum>(ANY_PACKAGE, TEXT("EGameStage"), true);
		if (StateEnum)
		{
			StageString = StateEnum->GetNameStringByValue((int64)StageNum);
		}
		UMainGameBlueprintFunctionLibrary::DebugTextPrint(GetWorld(), FString(TEXT("CurStage = ")) + StageString);
	}
#endif
	//DefaultRayCast(DeltaTime);
	//TArray<FItemInformation> I = ItemSlot;
	//AGameModeBase* Ptr = GetWorld()->GetAuthGameMode();
}

void ATestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// State, Posture, Action
	DOREPLIFETIME(ATestCharacter, StateValue);
	DOREPLIFETIME(ATestCharacter, PostureValue);	// => ���� ����.
	DOREPLIFETIME(ATestCharacter, LowerStateValue); // => ���� ����.
	DOREPLIFETIME(ATestCharacter, DirValue);		// => ���� ����.
	DOREPLIFETIME(ATestCharacter, IsFaint);			// 7/26 �߰�
	DOREPLIFETIME(ATestCharacter, IsBombSetting);	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)

	// Item
	DOREPLIFETIME(ATestCharacter, RayCastToItemName);

	// HP (for UI, Monster test)
	DOREPLIFETIME(ATestCharacter, PlayerHp);

	// Server?
	DOREPLIFETIME(ATestCharacter, Token); // => ���� ����.
}

void ATestCharacter::FireRayCast_Implementation() // => ���ε� �����ؾ� �� (24.07.25 ������) // Main ����.
{
	if (CurItemIndex == -1 || CurItemIndex == 2)
	{
		return;
	}

	if (ItemSlot[CurItemIndex].ReloadLeftNum <= 0)
	{
		ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
	}

	ATestPlayerController* Con = Cast<ATestPlayerController>(GetController());
	FVector Start = GetMesh()->GetSocketLocation(FName("MuzzleSocket"));
	//Start.Z -= 20.0f;
	FVector End = (Con->GetControlRotation().Vector() * 2000.0f) + Start;
	FHitResult Hit;

	if (GetWorld())
	{
		ItemSlot[CurItemIndex].ReloadLeftNum -= 1;
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Bullet left : %d / %d"), ItemSlot[CurItemIndex].ReloadLeftNum, ItemSlot[CurItemIndex].ReloadMaxNum));

		//bool ActorHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel9, FCollisionQueryParams(), FCollisionResponseParams());
		TArray<AActor*> IgnoreActors;
		bool ActorHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0.0f, 0.0f);

		if (true == ActorHit && nullptr != Hit.GetActor())
		{
			FString BoneName = Hit.BoneName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("Bone Name : %s"), *BoneName);
			{
				ATestMonsterBase* Monster = Cast<ATestMonsterBase>(Hit.GetActor()); // [Main] ABasicMonsterBase
				if (nullptr != Monster)
				{
					Monster->Damaged(ItemSlot[CurItemIndex].Damage);
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s got damage : %d"), *Monster->GetName(), ItemSlot[CurItemIndex].Damage));
				}
			}
			{
				ATestBossMonsterBase* BossMonster = Cast<ATestBossMonsterBase>(Hit.GetActor());
				if (nullptr != BossMonster)
				{
					BossMonster->Damaged(ItemSlot[CurItemIndex].Damage);
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s got damage : %d"), *BossMonster->GetName(), ItemSlot[CurItemIndex].Damage));
				}
			}
		}
	}
}

void ATestCharacter::Drink_Implementation()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
{
	// ���� �������� ���ٸ� return
	if (IsItemIn[3] == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("There's no item to drop. (Current posture is 'Barehand')")));
		return;
	}


}

void ATestCharacter::BombSetStart_Implementation()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
{
	// ��ź �������� ���ٸ� return


}

void ATestCharacter::BombSetCancel_Implementation()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
{

}

void ATestCharacter::BombSetComplete_Implementation()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
{

}

void ATestCharacter::ChangeMontage_Implementation(bool _IsFireEnd) // => ���� ����.
{
	if (_IsFireEnd == false)
	{
		switch (PostureValue)
		{
		case EPlayerPosture::Rifle1:
			UpperStateValue = EPlayerUpperState::Rifle_Attack;
			break;
		case EPlayerPosture::Rifle2:
			UpperStateValue = EPlayerUpperState::Rifle_Attack;
			break;
		case EPlayerPosture::Melee:
			UpperStateValue = EPlayerUpperState::Melee;
			break;
		case EPlayerPosture::Drink:
			UpperStateValue = EPlayerUpperState::Drink;
			break;
		case EPlayerPosture::Bomb:
			UpperStateValue = EPlayerUpperState::Bomb;
			break;
		case EPlayerPosture::Barehand:
			UpperStateValue = EPlayerUpperState::Barehand;
			break;
		case EPlayerPosture::SlotMax:
			break;
		default:
			break;
		}
		PlayerAnimInst->ChangeAnimation(UpperStateValue);
		FPVPlayerAnimInst->ChangeAnimation(UpperStateValue);
		ClientChangeMontage(false);
	}
	else // FireEnd
	{
		UpperStateValue = EPlayerUpperState::Rifle_Idle;
		PlayerAnimInst->ChangeAnimation(UpperStateValue);
		FPVPlayerAnimInst->ChangeAnimation(UpperStateValue);
		ClientChangeMontage(true);
	}
}

void ATestCharacter::ClientChangeMontage_Implementation(bool _IsFireEnd) // => ���� ����.
{
	if (_IsFireEnd == false)
	{
		switch (PostureValue)
		{
		case EPlayerPosture::Rifle1:
			UpperStateValue = EPlayerUpperState::Rifle_Attack;
			break;
		case EPlayerPosture::Rifle2:
			UpperStateValue = EPlayerUpperState::Rifle_Attack;
			break;
		case EPlayerPosture::Melee:
			UpperStateValue = EPlayerUpperState::Melee;
			break;
		case EPlayerPosture::Drink:
			UpperStateValue = EPlayerUpperState::Drink;
			break;
		case EPlayerPosture::Bomb:
			UpperStateValue = EPlayerUpperState::Bomb;
			break;
		case EPlayerPosture::Barehand:
			UpperStateValue = EPlayerUpperState::Barehand;
			break;
		case EPlayerPosture::SlotMax:
			break;
		default:
			break;
		}
		PlayerAnimInst->ChangeAnimation(UpperStateValue);
		FPVPlayerAnimInst->ChangeAnimation(UpperStateValue);
	}
	else // FireEnd
	{
		UpperStateValue = EPlayerUpperState::Rifle_Idle;
		PlayerAnimInst->ChangeAnimation(UpperStateValue);
		FPVPlayerAnimInst->ChangeAnimation(UpperStateValue);
	}
}

void ATestCharacter::ChangeState_Implementation(EPlayerState _Type)
{
	StateValue = _Type;
}

void ATestCharacter::ChangePosture_Implementation(EPlayerPosture _Type)	// => �������� �����ؾ� �� (24.07.25 ������) // Main�� ����.
{
	if (_Type == EPlayerPosture::Bomb || _Type == EPlayerPosture::Drink)	// Bomb, Drink => �ڼ������� �� ����
	{
		return;
	}
	else if (_Type == EPlayerPosture::Barehand)								// Barehand => �Ǽ� �ڼ��� ����
	{
		PostureValue = _Type;
		CurItemIndex = -1;

		// ������ �޽� visibility ����
		ItemSocketMesh->SetVisibility(false);
		FPVItemSocketMesh->SetVisibility(false);
	}
	else																	// Rifle1, Rifle2, Melee => ���⸦ �� �ڼ��� ����
	{
		int ItemSlotIndex = static_cast<int>(_Type);
		if (IsItemIn[ItemSlotIndex] == false)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("The item slot is empty."));
			return;
		}
		PostureValue = _Type;
		CurItemIndex = ItemSlotIndex;

		// ������ static mesh ����
		ItemSocketMesh->SetStaticMesh(ItemSlot[CurItemIndex].MeshRes);
		FPVItemSocketMesh->SetStaticMesh(ItemSlot[CurItemIndex].MeshRes);

		// ������ �޽� transform ����
		ItemSocketMesh->SetRelativeLocation(ItemSlot[CurItemIndex].RelLoc);
		FPVItemSocketMesh->SetRelativeLocation(ItemSlot[CurItemIndex].RelLoc);

		ItemSocketMesh->SetRelativeRotation(ItemSlot[CurItemIndex].RelRot);
		FPVItemSocketMesh->SetRelativeRotation(ItemSlot[CurItemIndex].RelRot);

		ItemSocketMesh->SetRelativeScale3D(ItemSlot[CurItemIndex].RelScale);
		FPVItemSocketMesh->SetRelativeScale3D(ItemSlot[CurItemIndex].RelScale);

		// ������ �޽� visibility �ѱ�
		ItemSocketMesh->SetVisibility(true);
		FPVItemSocketMesh->SetVisibility(true);
	}
}

void ATestCharacter::ChangeLowerState_Implementation(EPlayerLowerState _LowerState) // => ���� ����.
{
	LowerStateValue = _LowerState;
}

void ATestCharacter::ChangePlayerDir_Implementation(EPlayerMoveDir _Dir) // => ���� ����.
{
	DirValue = _Dir;
}

void ATestCharacter::ChangeIsFaint_Implementation()
{
	if (true == IsFaint)
	{
		IsFaint = false;
	}
	else
	{
		IsFaint = true;
	}
}

void ATestCharacter::CheckItem()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)
{
	// �ʿ� �������� ���� ���.
	if (nullptr == GetMapItemData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("There is no item to check."));
		return;
	}

	AMapObjectBase* GetMapItem = Cast<AMapObjectBase>(GetMapItemData);
	if (nullptr != GetMapItem)
	{
		// 1. �ʿ�����Ʈ�� ���
		InteractObject(GetMapItem);
	}
	else
	{
		// 2. �ֿ� �� �ִ� �������� ���
		PickUpItem();
	}
}

void ATestCharacter::InteractObject_Implementation(AMapObjectBase* _MapObject)	// => ����ĳ���ͷ� �����ؾ� �� (24.07.30 ���� ��)
{
	// Area�� ��� : ��ȣ�ۿ��� �÷��̾��ʿ��� ó���ؾ� �ϹǷ� return


	// Door�� ��� : ��ȣ�ۿ��� Switch�� �ߵ���Ű�Ƿ� return


	// �� �� �ʿ�����Ʈ(Switch ��)�� ��� : ��ȣ�ۿ� �ߵ�
	_MapObject->InterAction();

	// (�̰� ���ϴ� �ڵ��ΰ���?)
	ABomb* GetSampleData = Cast<ABomb>(_MapObject);
	if (nullptr != GetSampleData)
	{
		for (size_t i = 0; i < GetSampleData->Tags.Num(); i++)
		{
			FName GetItemTag = GetSampleData->Tags[i];
			if ("Sample" == GetItemTag)
			{
				GetSampleData->CharacterToDestroy();
			}
		}
	}
}

void ATestCharacter::PickUpItem_Implementation()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 ������)
{
	// Overlap�� �������� Tag �̸��� ���� FName�� �����´�.
	FString TagName = "";
	for (size_t i = 0; i < GetMapItemData->Tags.Num(); i++)
	{
		TagName = GetMapItemData->Tags[i].ToString();
	}
	FName ItemStringToName = FName(*TagName);			// ������ �̸�

	{
		if(false == IsItemIn[static_cast<int>(EPlayerPosture::Rifle1)])
		{
			// 1�� ������ ����ִ� ���.
			ItemSetting(ItemStringToName, false);
		}
		else if (true == IsItemIn[static_cast<int>(EPlayerPosture::Rifle1)] && true == IsItemIn[static_cast<int>(EPlayerPosture::Rifle2)])
		{
			// 1�� ������ �ְ� 2�� ������ ����ִ� ���.
			ItemSetting(ItemStringToName, true);
		}
		else
		{
			// 1, 2�� ������ ������� �ʴ� ���.
			if (PostureValue == EPlayerPosture::Rifle1)
			{
				DropItem();
				DeleteItem(static_cast<int>(PostureValue));
				ItemSetting(ItemStringToName, false);
			}
			else if (PostureValue == EPlayerPosture::Rifle2)
			{
				DropItem();
				DeleteItem(static_cast<int>(PostureValue));
				ItemSetting(ItemStringToName, true);
			}
		}
	}

	/*
	{
		// ItemName�� �´� ������ ������ DT���� �����´�.
		UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
		const FItemDataRow* ItemData = Inst->GetItemData(ItemStringToName);

		EPlayerPosture ItemType = ItemData->GetType();		// ������ Ÿ��

		// �̹� �κ��丮�� ���� �̸��� ���� �������� ���� ���.
		if (ItemStringToName == ItemSlot[static_cast<int>(ItemType)].Name)
		{
			GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("The same item is already in inventory."));
			return;
		}

		// �̹� �κ��丮�� ���� Ÿ���� �������� ���� ���. (���� ������ ���� ����)
		if (true == IsItemIn[static_cast<uint8>(ItemType)])
		{
			DropItem();
		}

		// Data Table�� �ִ� ������ ���� ��������.
		int ItemReloadNum = ItemData->GetReloadNum();		// ���� ���� ���� (30, 40)	// -1�� ��� �ѱ�� ���Ⱑ �ƴ�
		int ItemDamage = ItemData->GetDamage();				// ���� ���ݷ�				// 0�� ��� ���Ⱑ �ƴ�
		UStaticMesh* ItemMeshRes = ItemData->GetResMesh();	// ����ƽ �޽� ���ҽ�
		FVector ItemRelLoc = ItemData->GetRelLoc();			// ����ƽ �޽� ������Ʈ ����� ��ġ
		FRotator ItemRelRot = ItemData->GetRelRot();		// ����ƽ �޽� ������Ʈ ����� ȸ��
		FVector ItemRelScale = ItemData->GetRelScale();		// ����ƽ �޽� ������Ʈ ����� ũ��

		// �κ��丮�� ������ ����ֱ�. (����ƽ�޽÷� �������� �������� ��� ä��!!!)
		uint8 ItemIndex = static_cast<uint8>(ItemType);		// �������� ���� �κ��丮 �ε���
		IsItemIn[ItemIndex] = true;

		ItemSlot[ItemIndex].Name = ItemStringToName;
		ItemSlot[ItemIndex].ReloadMaxNum = ItemReloadNum;
		ItemSlot[ItemIndex].ReloadLeftNum = ItemReloadNum;
		ItemSlot[ItemIndex].Damage = ItemDamage;
		ItemSlot[ItemIndex].MeshRes = ItemMeshRes;
		ItemSlot[ItemIndex].RelLoc = ItemRelLoc;
		ItemSlot[ItemIndex].RelRot = ItemRelRot;
		ItemSlot[ItemIndex].RelScale = ItemRelScale;

		// ���� �÷��� ���� �ܰ� ������Ʈ
		if (EPlayerPosture::Rifle1 == ItemType
			|| EPlayerPosture::Melee == ItemType
			|| EPlayerPosture::Bomb == ItemType)
		{
			AMainGameState* CurGameState = UMainGameBlueprintFunctionLibrary::GetMainGameState(GetWorld());

			if (nullptr == CurGameState)
			{
				UE_LOG(PlayerLog, Fatal, TEXT("GameState Is Nullptr"));
				return;
			}

			switch (ItemType)
			{
			case EPlayerPosture::Rifle1:
			case EPlayerPosture::Bomb:
				CurGameState->AddArmoryWeaponNum(ItemType);
				break;
			case EPlayerPosture::Melee:
				CurGameState->AddMeleeNum();
				break;
			default:
				break;
			}
		}

		// ���� Type�� ���� �ִϸ��̼� ��ȭ �Լ� ȣ��.
		ChangePosture(ItemType);
	}
	*/

	// Map�� �ִ� ������ ����.
	GetMapItemData->Destroy();

	// Widget CallBack
	ATestPlayerController* Con = Cast<ATestPlayerController>(GetController());
	if (nullptr != Con)
	{
		Con->FGetItemToWidget_Test.Execute();
	}
}

void ATestCharacter::DropItem_Implementation()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 ������)
{
	// DropItem �� �� ���� ��� 1: �Ǽ��� ��
	if (CurItemIndex == -1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("There's no item to drop. (Current posture is 'Barehand')")));
		return;
	}

	// DropItem �� �� ���� ��� 2: (�׷����� ��������) ���� Posture�� �ش��ϴ� ���Ⱑ �κ��丮�� ���� ��
	if (IsItemIn[CurItemIndex] == false)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("There's no item to drop. (The item slot is empty)")));
		return;
	}

	// ����Ʈ�� �������� Actor�� ����
	FName ItemName = ItemSlot[CurItemIndex].Name;
	// UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld()); << ���ο��� �̷��� �����.
	UMainGameInstance* MainGameInst = GetWorld()->GetGameInstanceChecked<UMainGameInstance>();
	const FItemDataRow* ItemBase = MainGameInst->GetItemData(ItemName);
	FTransform BoneTrans = GetMesh()->GetBoneTransform(FName("RightHand"), ERelativeTransformSpace::RTS_World); // ���� ����.(07/29)

	// Spawn
	AActor* DropItem = GetWorld()->SpawnActor<AActor>(ItemBase->GetItemUClass(), BoneTrans);

	// �������� ������ ������ (�̿�)
	//GetMesh()->SetSimulatePhysics(true);
	FVector ImpulseVector = GetActorForwardVector() * 1000.0f;
	GetMesh()->AddImpulse(ImpulseVector, FName("RightHand"), false);

	// �տ� ��� �ִ� �������� �κ��丮���� ����
	DeleteItem(CurItemIndex);

	// �ڼ��� �Ǽ����� ����
	ChangePosture(EPlayerPosture::Barehand);
}

void ATestCharacter::ItemSetting(FName _TagName, bool _InNextSlotToItem)
{
	// ItemName�� �´� ������ ������ DT���� �����´�.
	UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
	const FItemDataRow* ItemData = Inst->GetItemData(_TagName);
	//EPlayerPosture ItemType = ItemData->GetType(); // ���⿡ ���� ��� ���߾�� ������ ������ �������鼭 �ǹ̰� ������.
	EPlayerPosture ItemType = PostureValue;

	// Data Table�� �ִ� ������ ���� ��������.
	int ItemReloadNum = ItemData->GetReloadNum();		// ���� ���� ���� (30, 40)	// -1�� ��� �ѱ�� ���Ⱑ �ƴ�
	int ItemDamage = ItemData->GetDamage();				// ���� ���ݷ�				// 0�� ��� ���Ⱑ �ƴ�
	UStaticMesh* ItemMeshRes = ItemData->GetResMesh();	// ����ƽ �޽� ���ҽ�
	FVector ItemRelLoc = ItemData->GetRelLoc();			// ����ƽ �޽� ������Ʈ ����� ��ġ
	FRotator ItemRelRot = ItemData->GetRelRot();		// ����ƽ �޽� ������Ʈ ����� ȸ
	FVector ItemRelScale = ItemData->GetRelScale();		// ����ƽ �޽� ������Ʈ ����� ũ��

	uint8 ItemIndex = static_cast<uint8>(ItemType);		// �������� ���� �κ��丮 �ε���
	if (true == _InNextSlotToItem)
	{
		ItemIndex += 1;
	}
	IsItemIn[ItemIndex] = true;

	ItemSlot[ItemIndex].Name = _TagName;
	ItemSlot[ItemIndex].ReloadMaxNum = ItemReloadNum;
	ItemSlot[ItemIndex].ReloadLeftNum = ItemReloadNum;
	ItemSlot[ItemIndex].Damage = ItemDamage;
	ItemSlot[ItemIndex].MeshRes = ItemMeshRes;
	ItemSlot[ItemIndex].RelLoc = ItemRelLoc;
	ItemSlot[ItemIndex].RelRot = ItemRelRot;
	ItemSlot[ItemIndex].RelScale = ItemRelScale;

	// ���� Type�� ���� �ִϸ��̼� ��ȭ �Լ� ȣ��.
	if (true == _InNextSlotToItem)
	{
		ChangePosture(EPlayerPosture::Rifle2);
	}
	else
	{
		ChangePosture(EPlayerPosture::Rifle1);
	}
}

void ATestCharacter::DeleteItem(int _Index)
{
	FItemInformation NewSlot;
	ItemSlot[_Index] = NewSlot;
	IsItemIn[_Index] = false;
}

void ATestCharacter::ChangePOV()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 ���� ��)
{
	if (IsFPV)	// ����Ī -> ����Ī
	{
		// SpringArm Component ��ġ ����.
		//SpringArmComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SpringArmComponent->TargetArmLength = 300.0f;
		SpringArmComponent->SetRelativeLocation(CameraRelLoc);

		// Character Mesh ��ȯ.
		GetMesh()->SetOwnerNoSee(false);
		FPVMesh->SetOwnerNoSee(true);

		// Item Mesh
		for (int i = 0; i < int(EPlayerPosture::Barehand); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(false);
			FPVItemSocketMesh->SetOwnerNoSee(true);
		}

		IsFPV = false;
	}
	else	// ����Ī -> ����Ī
	{
		// SpringArm Component ��ġ ����.
		//SpringArmComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, FName("SpringArmSocket"));
		SpringArmComponent->TargetArmLength = 0.0f;
		switch (LowerStateValue)
		{
		case EPlayerLowerState::Idle:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc);
			break;
		case EPlayerLowerState::Crouch:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc_Crouch);
			break;
		default:
			break;
		}

		// Character Mesh ��ȯ.
		GetMesh()->SetOwnerNoSee(true);
		FPVMesh->SetOwnerNoSee(false);

		// Item Mesh
		for (int i = 0; i < int(EPlayerPosture::Barehand); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(true);
			FPVItemSocketMesh->SetOwnerNoSee(false);
		}

		IsFPV = true;
	}
}

void ATestCharacter::CharacterReload() // => ���� ����.
{
	if (-1 == CurItemIndex)
	{
		return;
	}
	ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
}

void ATestCharacter::MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision) // => ���� ����.
{
	GetMapItemData = _OtherActor;
}

void ATestCharacter::MapItemOverlapEnd() // => ���� ����.
{
	if (nullptr != GetMapItemData)
	{
		GetMapItemData = nullptr;
	}
}

void ATestCharacter::CrouchCameraMove() // => ���ο� ���� �ʿ� (24.07.29 ������) => ���� ����.
{
	if (IsFPV)
	{
		switch (LowerStateValue)
		{
		case EPlayerLowerState::Idle:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc_Crouch);
			break;
		case EPlayerLowerState::Crouch:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc);
			break;
		default:
			break;
		}
	}
}

void ATestCharacter::NetCheck() // => ���� ����.
{
	IsServer = GetWorld()->GetAuthGameMode() != nullptr;
	IsClient = !IsServer;

	if (true == IsServer)
	{
		IsCanControlled = (GetLocalRole() == ROLE_Authority) ? true : false;
	}
	else // client
	{
		IsCanControlled = (GetLocalRole() == ROLE_AutonomousProxy) ? true : false;
	}

	if (true == IsServer)
	{
		UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
		// ����ū�� �� �ε����� �ƴϴ�.
		Token = Inst->GetNetToken();
		MyMaxHp = Inst->GetPlayerData(FName("TestPlayer"))->GetHp();

		// UGameplayStatics::GetPlayerPawn(Token)
	}
	else // client
	{

	}
}

void ATestCharacter::SendTokenToHpBarWidget() // => ���� ���� ���� ��.(HUD, Widget �����.)
{
	ATestPlayerController* Con = Cast<ATestPlayerController>(GetController());
	if (nullptr == Con)
	{
		return;
	}
	ATestPlayHUD* PlayHUD = Cast<ATestPlayHUD>(Con->GetHUD());
	if (nullptr == PlayHUD)
	{
		return;
	}
	UTestHpBarUserWidget* MyHpWidget = Cast<UTestHpBarUserWidget>(PlayHUD->GetWidget(EUserWidgetType::HpBar));
	if (nullptr == MyHpWidget)
	{
		return;
	}

	if (true == IsCanControlled && -1 != Token)
	{
		MyHpWidget->HpbarInit_ForMainPlayer(Token);
	}
	else
	{
		int a = 0;
		return;
	}
}

void ATestCharacter::UpdatePlayerHp(float _DeltaTime) // => ���� ���� ���� ��.(HUD, Widget �����.)
{
	ATestPlayerState* MyTestPlayerState = GetPlayerState<ATestPlayerState>();
	if (nullptr == MyTestPlayerState)
	{
		return;
	}

	ATestPlayerController* MyController = Cast<ATestPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (nullptr == MyController)
	{
		return;
	}


	float GetHp = MyTestPlayerState->GetPlayerHp();

	//CurHp = MyTestPlayerState->GetPlayerHp();

	ATestPlayHUD* PlayHUD = Cast<ATestPlayHUD>(MyController->GetHUD());
	if (nullptr != PlayHUD && Token != -1)
	{
		UTestHpBarUserWidget* MyHpWidget = Cast<UTestHpBarUserWidget>(PlayHUD->GetWidget(EUserWidgetType::HpBar));
		FString TestName = "CORORO_" + FString::FromInt(Token);
		MyHpWidget->NickNameUpdate(Token, FText::FromString(TestName));
		MyHpWidget->HpbarUpdate(Token, GetHp, 100.0f);
	}
}

void ATestCharacter::SettingPlayerState_Implementation()
{
	ATestPlayerController* Con = Cast<ATestPlayerController>(GetController());
	if (nullptr == Con)
	{
		int a = 0;
		return;
	}

	ATestPlayerState* ThisPlayerState = Cast<ATestPlayerState>(Con->PlayerState);
	if (nullptr == ThisPlayerState)
	{
		int a = 0;
		return;
	}

	ThisPlayerState->InitPlayerData();
}
