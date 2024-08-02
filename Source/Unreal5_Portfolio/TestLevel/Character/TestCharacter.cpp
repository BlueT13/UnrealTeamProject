// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Camera/CameraShakeBase.h"
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
#include "PartDevLevel/Monster/Base/TestMonsterBase.h"
#include "PartDevLevel/Monster/Boss/TestBossMonsterBase.h"
#include "MainGameLevel/Object/MapObjectBase.h"
#include "MainGameLevel/Object/Bomb.h"
#include "MainGameLevel/Object/DoorObject.h"
#include "MainGameLevel/Object/AreaObject.h"
#include "MainGameLevel/UI/InGame/HeadNameWidgetComponent.h"
#include "TestPlayerController.h"
#include "MainGameLevel/Particles/MuzzleParticleActor.h"
#include "PartDevLevel/Monster/Kraken/KrakenProjectile.h"

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

	// FPV Character Mesh	// => ���� ���� �ʿ� (24.08.02 ������)
	FPVMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPVMesh->SetupAttachment(CameraComponent);
	FPVMesh->SetRelativeLocation(FVector(-10.0f, 0.0f, -160.0f));
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

	// Map Item �˻�		// => ���� ���� �ʿ� (24.08.01 ������)
	GetMapItemCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("GetMapItemCollisionComponent"));
	GetMapItemCollisionComponent->SetupAttachment(RootComponent);
	GetMapItemCollisionComponent->SetRelativeLocation(FVector(60.0, 0.0, -5.0f));
	GetMapItemCollisionComponent->SetBoxExtent(FVector(55.0f, 50.0f, 100.0f));
	GetMapItemCollisionComponent->SetCollisionProfileName(FName("MapItemSearch"));

	UEnum* Enum = StaticEnum<EPlayerPosture>();

	// Inventory (for UI Test)	// => ����ĳ���� ����. [�ּ� �κ� �ٸ��� Ȯ�� ��û.]
	for (size_t i = 0; i < static_cast<size_t>(EPlayerPosture::Barehand); i++)
	{
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
	HandAttackComponent->SetRelativeLocation({ 0.0f, 100.0f, 120.0f });
	HandAttackComponent->SetCollisionProfileName(TEXT("NoCollision"));	// => ���� ���� �ʿ� (24.08.01 BeginPlay �Լ����� ����� �̵�)

	// MinimapIcon Component => ����ĳ���� ����.
	MinimapIconComponent = CreateDefaultSubobject<UTestMinimapIconComponent>(TEXT("MinimapPlayerIcon"));
	MinimapIconComponent->SetupAttachment(RootComponent);
	MinimapIconComponent->bVisibleInSceneCaptureOnly = true;

	// HeadName Component	// => �������� ���� �ʿ� (24.07.30 �߰���)
	HeadNameComponent = CreateDefaultSubobject<UHeadNameWidgetComponent>(TEXT("HeadNameWidgetComponent"));
	HeadNameComponent->SetupAttachment(RootComponent);
	HeadNameComponent->SetOwnerNoSee(true);
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


	// Kraken�� ���� �νô� �Լ� ȣ�� (���� �߰� �ʿ�)
	{
		AKrakenProjectile* Rock = Cast<AKrakenProjectile>(_OtherActor);
		if (nullptr != Rock)
		{
			Rock->Damaged(150.0f);
		}
	}
}

void ATestCharacter::ChangeHandAttackCollisionProfile(FName _Name) // => ���� ����.
{
	HandAttackComponent->SetCollisionProfileName(_Name);
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
		USkeletalMesh* PlayerSkeletalMesh = MainGameInst->GetPlayerData(FName("Vanguard"))->GetPlayerSkeletalMesh();
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh);

		// �� �޽� ���� (���� �߰� �ʿ�)
		USkeletalMesh* FPVSkeletalMesh = MainGameInst->GetPlayerData(FName("Vanguard"))->GetPlayerFPVPlayerSkeletalMesh();
		FPVMesh->SetSkeletalMesh(FPVSkeletalMesh);

		// ABP ����
		UClass* AnimInst = Cast<UClass>(MainGameInst->GetPlayerData(FName("Vanguard"))->GetPlayerAnimInstance());
		GetMesh()->SetAnimInstanceClass(AnimInst);
	}

	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void ATestCharacter::BeginPlay()	// => ���� ���� �ʿ� (24.08.01 ����, �߰��� ��� ����)
{
	NetCheck();
	Super::BeginPlay();

	UMainGameBlueprintFunctionLibrary::PushActor(EObjectType::Player, this);

	// PlayerState => �ʱ�ȭ
	SettingPlayerState();

	// ��Ÿ�� ���濡 �ʿ��� ���� �߰� �ʿ� (��ȯ) // => ���� ����.
	PlayerAnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	FPVPlayerAnimInst = Cast<UPlayerAnimInstance>(FPVMesh->GetAnimInstance());

	// GetMapItemCollisionComponent => MapItem�� Overlap�� �� ������ �Լ� ���ε�
	GetMapItemCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATestCharacter::MapItemOverlapStart);
	GetMapItemCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATestCharacter::MapItemOverlapEnd);

	//UISetting();
}

// Called every frame
void ATestCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerHp(DeltaTime); // => ���� ����.

	// �ٸ� �÷��̾��� HeadNameComponent�� �׻� ���� ���ϵ��� ȸ��	// => ���� ���� �ʿ� (24.07.30 �߰���)
	if (nullptr != HeadNameComponent)
	{
		HeadNameComponent->BilboardRotate(GetActorLocation());
	}

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
	// GameState ���� ��¿� TestCode (�̰� ���ο� �ű���� �ʾƵ� �˴ϴ�!! - ���� -)
	{
		AMainGameState* CurGameState = UMainGameBlueprintFunctionLibrary::GetMainGameState(GetWorld());

		if (nullptr == CurGameState)
		{
			return;
		}
		int CurPlayerNum = CurGameState->GetPlayerCount();
		FString PNString = FString::FromInt(CurPlayerNum);
		UMainGameBlueprintFunctionLibrary::DebugTextPrint(GetWorld(), FString(TEXT("CurPlayerCount = ")) + PNString);

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
	//float ts = GetWorld()->GetDeltaSeconds();
	
	//int a = 0;
}

void ATestCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Posture, Action
	DOREPLIFETIME(ATestCharacter, PostureValue);	// => ���� ����.
	DOREPLIFETIME(ATestCharacter, PrevPostureValue);
	DOREPLIFETIME(ATestCharacter, LowerStateValue); // => ���� ����.
	DOREPLIFETIME(ATestCharacter, DirValue);		// => ���� ����.
	DOREPLIFETIME(ATestCharacter, IsFaint);			// 7/26 �߰�
	DOREPLIFETIME(ATestCharacter, IsBombSetting);	// => ���ο� ���� �ʿ� (24.07.29 �߰���)

	// Item
	DOREPLIFETIME(ATestCharacter, RayCastToItemName);

	// Server?
	DOREPLIFETIME(ATestCharacter, Token); // => ���� ����.
}

void ATestCharacter::FireRayCast_Implementation() // => ���� ���� �ʿ� (24.07.30 DebugMessage �κ� ������)
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
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("Bullet left : %d / %d"), ItemSlot[CurItemIndex].ReloadLeftNum, ItemSlot[CurItemIndex].ReloadMaxNum));
#endif // WITH_EDITOR

		//bool ActorHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_GameTraceChannel9, FCollisionQueryParams(), FCollisionResponseParams());
		TArray<AActor*> IgnoreActors;
		bool ActorHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 1.0f, 0.0f, 0.0f);

		if (true == ActorHit && nullptr != Hit.GetActor())
		{
			FString BoneName = Hit.BoneName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("Bone Name : %s"), *BoneName);
			{
				AKrakenProjectile* Rock = Cast<AKrakenProjectile>(Hit.GetActor()); // [Main] ABasicMonsterBase
				if (nullptr != Rock)
				{
					Rock->Damaged(ItemSlot[CurItemIndex].Damage);
					GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%s got damage : %d"), *Rock->GetName(), ItemSlot[CurItemIndex].Damage));
				}
			}
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

void ATestCharacter::Drink_Implementation()					// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	// ���� �������� ���ٸ� return
	if (false == IsItemIn[3])
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), 4));
#endif
		return;
	}

	// �ִϸ��̼� ����
	ChangePosture(EPlayerPosture::Drink);

	// ���� ������ ����
	DeleteItem(3);

	// �������� �÷��̾� HP ȸ��
	ATestPlayerState* MyTestPlayerState = GetPlayerState<ATestPlayerState>();
	if (nullptr == MyTestPlayerState)
	{
		return;
	}
	MyTestPlayerState->HealHp();

#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("HP recovered!")));
#endif
}

void ATestCharacter::DrinkComplete_Implementation()			// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	// ���� �ڼ��� �ִϸ��̼� ����
	ChangePosture(PrevPostureValue);
}

void ATestCharacter::BombSetStart_Implementation()			// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	// ��ź �������� ���ٸ� return
	if (false == IsItemIn[4])
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), 5));
#endif
		return;
	}

	// 240801 AreaObject �߰��� �ش� Ŭ���� ����
	// ��ź ��ġ ������ Area�� �ƴ� ��� return
	//ATestArea* AreaObject = Cast<ATestArea>(GetMapItemData);
	AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);
	if (nullptr == AreaObject)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("You have to set the bomb in proper area.")));
#endif
		return;
	}

	// ��ź ��ġ ������ ������ �Ǵ�, ��ġ ����
	IsBombSetting = true;
	AreaObject->ResetBombTime();
#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString(TEXT("Bomb setting start.")));
#endif

	// �ִϸ��̼� ����
	ChangePosture(EPlayerPosture::Bomb);
}

void ATestCharacter::BombSetTick_Implementation()		// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	if (true == IsBombSetting)
	{
		// 240801 AreaObject �߰��� �ش� Ŭ���� ����
		AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);
		//ATestArea* AreaObject = Cast<ATestArea>(GetMapItemData);
		if (nullptr == AreaObject)
		{
#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("You have to set the bomb in proper area.")));
#endif
			return;
		}

		// ��ġ �ð� ī��Ʈ�� ������ ���
		if (0 >= AreaObject->GetInstallBombTime())
		{
			BombSetComplete();
		}

		// ��ġ �ð� ī����
		AreaObject->InstallBomb(GetWorld()->GetDeltaSeconds());
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString::Printf(TEXT("%f seconds left..."), AreaObject->GetInstallBombTime()));
#endif
	}
}

void ATestCharacter::BombSetCancel_Implementation()		// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	if (true == IsBombSetting)
	{
		// ��ź ��ġ �ߴ�
		IsBombSetting = false;
		// 240801 AreaObject �߰��� �ش� Ŭ���� ����
		AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);
		//ATestArea* AreaObject = Cast<ATestArea>(GetMapItemData);
		if (nullptr != AreaObject)
		{
			AreaObject->ResetBombTime();
		}

		// ���� �ڼ��� �ִϸ��̼� ����
		ChangePosture(PrevPostureValue);
	}
}

void ATestCharacter::BombSetComplete_Implementation()	// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	// ��ź ��ġ �Ϸ�
	IsBombSetting = false;

	// 240801 AreaObject �߰��� �ش� Ŭ���� ����
	AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);
	//ATestArea* AreaObject = Cast<ATestArea>(GetMapItemData);
	if (nullptr != AreaObject)
	{
		AreaObject->InterAction();
	}
#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, FString(TEXT("Bomb setting completed!")));
#endif // WITH_EDITOR

	// ��ź ������ ����
	DeleteItem(4);

	// ���� �ڼ��� �ִϸ��̼� ����
	ChangePosture(PrevPostureValue);
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

void ATestCharacter::ChangePosture_Implementation(EPlayerPosture _Type)	// => �������� �����ؾ� �� (24.07.30 ���� ��)
{
	if (_Type == EPlayerPosture::Barehand)			
	{
		// 1. Barehand => �Ǽ� �ڼ��� ����
		PrevPostureValue = PostureValue;
		PostureValue = _Type;
		CurItemIndex = -1;

		// ������ �޽� visibility ����
		ItemSocketMesh->SetVisibility(false);
		FPVItemSocketMesh->SetVisibility(false);
	}
	else
	{
		// 2. Rifle1, Rifle2, Melee, Drink, Bomb => �������� �� �ڼ��� ����
		int ItemSlotIndex = static_cast<int>(_Type);
		if (IsItemIn[ItemSlotIndex] == false)
		{
#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), ItemSlotIndex + 1));
#endif // WITH_EDITOR
			return;
		}
		PrevPostureValue = PostureValue;
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

void ATestCharacter::CheckItem()	// => ���� ���� �ʿ� (24.08.02 PickUpItem ���� �߰���)
{
	// �ʿ� �������� ���� ���.
	if (nullptr == GetMapItemData)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There is no item to check.")));
#endif // WITH_EDITOR
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
		PickUpItem(GetMapItemData);
	}
}

void ATestCharacter::InteractObject_Implementation(AMapObjectBase* _MapObject)	// => ���� ���� �ʿ� (24.08.01 ���� ��)
{
	// Door�� ��� : ��ȣ�ۿ��� Switch�� �ߵ���Ű�Ƿ� return
	ADoorObject* DoorObject = Cast<ADoorObject>(_MapObject);
	if (nullptr != DoorObject)
	{
		return;
	}

	// Area�� ��� : ��ȣ�ۿ��� �÷��̾��ʿ��� ó���ؾ� �ϹǷ� return
	//ATestArea* AreaObject = Cast<ATestArea>(_MapObject);
	//if (nullptr != AreaObject)
	//{
	//	return;
	//}

	// 240801 AreaObject �߰��� �ش� ���� �߰��Ǿ����ϴ�
	// Area�� ��� :��ȣ�ۿ��� �÷��̾��ʿ��� ó���ؾ� �ϹǷ� return
	AAreaObject* AreaObject = Cast<AAreaObject>(_MapObject);
	if (nullptr != AreaObject)
	{
		return;
	}

	//// Bomb�� ��� : �κ��丮�� ������ ����ֱ�, �ʿ��� �������� ����
	//ABomb* BombObject = Cast<ABomb>(_MapObject);
	//if (nullptr != BombObject)
	//{
	//	PickUpItem();
	//	return;
	//}

	// �� �� �ʿ�����Ʈ(Switch ��)�� ��� : ��ȣ�ۿ� �ߵ�
	_MapObject->InterAction();
}

void ATestCharacter::PickUpItem_Implementation(AActor* _Item)	// => ���� ���� �ʿ� (24.08.02 ���� �߰��� ���� TagName �������� �κ� ������)
{
	// Overlap�� �������� Tag �̸��� ���� FName�� �����´�.
	FString TagName = "";
	for (size_t i = 0; i < _Item->Tags.Num(); i++)
	{
		TagName = _Item->Tags[i].ToString();
	}
	FName ItemStringToName = FName(*TagName);			// ������ �̸�

	UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
	const FItemDataRow* ItemData = Inst->GetItemData(ItemStringToName);

	EPlayerPosture ItemType = ItemData->GetType();		// ������ Ÿ��

	if (ItemType == EPlayerPosture::Rifle1)	// Rifle �������� �ݴ� ���
	{
		if (false == IsItemIn[static_cast<int>(EPlayerPosture::Rifle1)])
		{
			// 1�� ������ ����ִ� ��� => 1�� ����
			ItemSetting(ItemStringToName, 0);
		}
		else if (true == IsItemIn[static_cast<int>(EPlayerPosture::Rifle1)] && false == IsItemIn[static_cast<int>(EPlayerPosture::Rifle2)])
		{
			// 1�� ������ ������� �ʰ�, 2�� ���Ը� ����ִ� ��� => 2�� ����
			ItemSetting(ItemStringToName, 1);
		}
		else
		{
			// 1�� ���Ե� 2�� ���Ե� ������� ���� ���
			if (PostureValue == EPlayerPosture::Rifle1 || PostureValue == EPlayerPosture::Rifle2)
			{
				// �������� �ݱ� ������ 1�� �Ǵ� 2�� �������� �տ� ��� �־��ٸ� => ���� 1�� �Ǵ� 2�� ����
				ItemSetting(ItemStringToName, static_cast<int>(PostureValue));
			}
			else
			{
				// �������� �ݱ� ������ �Ǽ��̰ų�, 3�� �������� �տ� ��� �־��ٸ� => 1�� ����
				ItemSetting(ItemStringToName, 0);
			}
		}
	}
	else	// Rifle�� �ƴ� �������� �ݴ� ���
	{
		ItemSetting(ItemStringToName, static_cast<int>(ItemType));
	}
	/*
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
	}
	*/

	// Widget CallBack
	ATestPlayerController* Con = Cast<ATestPlayerController>(GetController());
	if (nullptr != Con)
	{
		Con->FGetItemToWidget_Test.Execute();
	}
}

void ATestCharacter::ItemSetting(FName _TagName, int _SlotIndex) // => ���� ���� �ʿ� (24.07.31 ������)
{
	// ItemName�� �´� ������ ������ DT���� �����´�.
	UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
	const FItemDataRow* ItemData = Inst->GetItemData(_TagName);
	EPlayerPosture ItemType = static_cast<EPlayerPosture>(_SlotIndex);

	// �̹� �κ��丮�� ���� �̸��� ���� �������� ���� ���.
	if (ItemSlot[_SlotIndex].Name == _TagName)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("The same item is already in inventory. (Index : %d)"), _SlotIndex + 1));
#endif // WITH_EDITOR
		return;
	}

	// �̹� �κ��丮�� ���� Ÿ���� �������� ���� ���.
	if (IsItemIn[_SlotIndex] == true)
	{
		DropItem(_SlotIndex);
	}

	// Data Table�� �ִ� ������ ���� ��������.
	int ItemReloadNum = ItemData->GetReloadNum();		// ���� ���� ���� (30, 40)	// -1�� ��� �ѱ�� ���Ⱑ �ƴ�
	int ItemDamage = ItemData->GetDamage();				// ���� ���ݷ�				// 0�� ��� ���Ⱑ �ƴ�
	UStaticMesh* ItemMeshRes = ItemData->GetResMesh();	// ����ƽ �޽� ���ҽ�
	FVector ItemRelLoc = ItemData->GetRelLoc();			// ����ƽ �޽� ������Ʈ ����� ��ġ
	FRotator ItemRelRot = ItemData->GetRelRot();		// ����ƽ �޽� ������Ʈ ����� ȸ
	FVector ItemRelScale = ItemData->GetRelScale();		// ����ƽ �޽� ������Ʈ ����� ũ��

	// �κ��丮�� ������ �ֱ�.
	IsItemIn[_SlotIndex] = true;

	ItemSlot[_SlotIndex].Name = _TagName;
	ItemSlot[_SlotIndex].ReloadMaxNum = ItemReloadNum;
	ItemSlot[_SlotIndex].ReloadLeftNum = ItemReloadNum;
	ItemSlot[_SlotIndex].Damage = ItemDamage;
	ItemSlot[_SlotIndex].MeshRes = ItemMeshRes;
	ItemSlot[_SlotIndex].RelLoc = ItemRelLoc;
	ItemSlot[_SlotIndex].RelRot = ItemRelRot;
	ItemSlot[_SlotIndex].RelScale = ItemRelScale;

#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Picked up new item! (Index : %d)"), _SlotIndex + 1));
#endif // WITH_EDITOR

	// Map�� �ִ� ������ ����.
	GetMapItemData->Destroy();

	// ������ Type�� ���� �ִϸ��̼� �ڼ� ��ȭ. (ItemType�� Rifle1, Rifle2, Melee�� ��츸)
	if (ItemType == EPlayerPosture::Rifle1 || ItemType == EPlayerPosture::Rifle2 || ItemType == EPlayerPosture::Melee)
	{
		ChangePosture(ItemType);
	}
}

void ATestCharacter::DropItem_Implementation(int _SlotIndex) // => ���� ���� �ʿ� (24.08.02 ������)
{
	// DropItem �� �� ���� ��� 1: �Ǽ��� ��
	if (CurItemIndex == -1)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There's no item to drop. (Current posture is 'Barehand')")));
#endif // WITH_EDITOR
		return;
	}

	// DropItem �� �� ���� ��� 2: (�׷����� ��������) ���� Posture�� �ش��ϴ� ���Ⱑ �κ��丮�� ���� ��
	if (IsItemIn[CurItemIndex] == false)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There's no item to drop. (The item slot is empty)")));
#endif // WITH_EDITOR
		return;
	}

	// ����Ʈ�� �������� Actor�� ����
	FName ItemName = ItemSlot[CurItemIndex].Name;
	// UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld()); << ���ο��� �̷��� �����.
	UMainGameInstance* MainGameInst = GetWorld()->GetGameInstanceChecked<UMainGameInstance>();
	const FItemDataRow* ItemBase = MainGameInst->GetItemData(ItemName);
	//FTransform BoneTrans = GetMesh()->GetBoneTransform(FName("RightHand"), ERelativeTransformSpace::RTS_World);
	FTransform SpawnTrans = GetActorTransform();
	SpawnTrans.SetTranslation(GetActorLocation() + (GetActorForwardVector() * 100.0f) + (GetActorUpVector() * 50.0f));

	// Spawn
	AItemBase* DropItem = GetWorld()->SpawnActor<AItemBase>(ItemBase->GetItemUClass(), SpawnTrans);
	//DropItem->GetStaticMeshComponent()->SetSimulatePhysics(true);

	// �������� ������ ������ (�̿�)
	//GetMesh()->SetSimulatePhysics(true);
	FVector ImpulseVector = GetActorForwardVector() * 1000.0f;
	GetMesh()->AddImpulse(ImpulseVector, FName("RightHand"), false);

	// �տ� ��� �ִ� �������� �κ��丮���� ����
	DeleteItem(CurItemIndex);

	// �ڼ��� �Ǽ����� ����
	ChangePosture(EPlayerPosture::Barehand);
}

void ATestCharacter::DeleteItem(int _Index)
{
	FItemInformation NewSlot;
	ItemSlot[_Index] = NewSlot;
	IsItemIn[_Index] = false;
}

void ATestCharacter::ChangePOV()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 ���� ��) => ���� ����.
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
	
	/* // Main
	// Widget �����
	Reload_Widget->SetVisibility(ESlateVisibility::Hidden);

	// �Ѿ� ������ ����.
	ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;

	// ����� �Ѿ� ������ ȣ��.
	AMainPlayerController* Con = Cast<AMainPlayerController>(GetController());
	if (nullptr != Con)
	{
		Con->FCharacterToReload.Execute(); // Execute -> Delegate ����.
	}
	*/
}

void ATestCharacter::MapItemOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)	// => ���� ���� �ʿ� (24.08.02 AreaObject�� ��� ���� �˻� �κ� ����)
{
	GetMapItemData = OtherActor;

	ATestPlayerController* MyController = Cast<ATestPlayerController>(GetController());
	if (nullptr == MyController)
	{
		return;
	}

	ATestPlayHUD* PlayHUD = Cast<ATestPlayHUD>(MyController->GetHUD());
	if (nullptr == PlayHUD)
	{
		return;
	}

	AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);
	if (nullptr != AreaObject)
	{
		// Area�� ��� => "5��Ű�� ���� ��ȣ�ۿ�"
		PlayHUD->UIOn(EUserWidgetType::Num5_Key);
		return;
	}
	
	// �� ���� ��� => "EŰ�� ���� ��ȣ�ۿ�"
	PlayHUD->UIOn(EUserWidgetType::E_Key);
}

void ATestCharacter::MapItemOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)	// => ���� ���� �ʿ� (24.08.01 ������)
{
	if (nullptr != GetMapItemData)
	{
		GetMapItemData = nullptr;
	}

	ATestPlayerController* MyController = Cast<ATestPlayerController>(GetController());
	if (nullptr == MyController)
	{
		return;
	}

	ATestPlayHUD* PlayHUD = Cast<ATestPlayHUD>(MyController->GetHUD());
	if (nullptr == PlayHUD)
	{
		return;
	}

	// ���� �ִ� ��ȣ�ۿ� ���� UI ��� ����
	PlayHUD->UIOff(EUserWidgetType::Num5_Key);
	PlayHUD->UIOff(EUserWidgetType::E_Key);
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
		//MyMaxHp = Inst->GetPlayerData(FName("TestPlayer"))->GetHp();

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

void ATestCharacter::ShowMuzzle()
{
	FTransform CreatePosition = GetMesh()->GetBoneTransform(FName("RightHand"), ERelativeTransformSpace::RTS_World);
	GetWorld()->SpawnActor<AActor>(AMuzzleParticleActor::StaticClass(), CreatePosition);
}
