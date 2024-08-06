// Fill out your copyright notice in the Description page of Project Settings.


#include "PartDevLevel/Character/TestFPVCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"
#include "TimerManager.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/DataTable/ItemDataRow.h"
#include "PartDevLevel/Monster/Base/TestMonsterBase.h"
#include "PartDevLevel/Character/TestFPVPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Global/DataTable/ItemDataRow.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"

#include "PartDevLevel/Monster/Boss/TestBossMonsterBase.h"
#include "PartDevLevel/Monster/Kraken/KrakenProjectile.h"
#include "PartDevLevel/Character/PlayerAnimInstance.h"

#include "TestLevel/Character/TestPlayerState.h"
#include "TestLevel/UI/TestMinimapIconComponent.h"
#include "TestLevel/UI/TestPlayHUD.h"
#include "TestLevel/UI/TestHpBarUserWidget.h"
#include "MainGameLevel/UI/InGame/HeadNameWidgetComponent.h"
#include "MainGameLevel/Object/ItemBase.h"
#include "MainGameLevel/Object/AreaObject.h"
#include "MainGameLevel/Object/DoorObject.h"
#include "MainGameLevel/Particles/MuzzleParticleActor.h"

// Sets default values
ATestFPVCharacter::ATestFPVCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Character Mesh => ����ĳ���� ���� �ʿ� (24.07.29 ������) => ���� ����.
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->SetRelativeLocation(FVector(0.0f, 0.0f, -88.0f));
	GetMesh()->bHiddenInSceneCapture = true;

	// Item Mesh			// => ����ĳ���� ����.
	ItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemSocketMesh"));
	ItemSocketMesh->SetupAttachment(GetMesh(), FName("ItemSocket"));
	ItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSocketMesh->SetGenerateOverlapEvents(true);
	ItemSocketMesh->SetOwnerNoSee(true);
	ItemSocketMesh->SetVisibility(false);
	ItemSocketMesh->SetIsReplicated(true);
	ItemSocketMesh->bHiddenInSceneCapture = true;

	// SpringArm Component	// => ����ĳ���� ���� �ʿ� (24.07.29 ������)
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc);
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = true;

	// Camera Component		// => ����ĳ���� ���� �ʿ� (24.07.29 ������)
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

	// FPV Item Mesh		// => ����ĳ���� ����.
	FPVItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPVItemSocketMesh"));
	FPVItemSocketMesh->SetupAttachment(FPVMesh, FName("FPVItemSocket"));
	FPVItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	FPVItemSocketMesh->SetGenerateOverlapEvents(true);
	FPVItemSocketMesh->SetOnlyOwnerSee(true);
	FPVItemSocketMesh->SetVisibility(false);
	FPVItemSocketMesh->SetIsReplicated(true);
	FPVItemSocketMesh->bCastDynamicShadow = false;
	FPVItemSocketMesh->CastShadow = false;

	// Map Item �˻�			// => ���� ���� �ʿ� (24.08.01 ������)
	GetMapItemCollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("GetMapItemCollisionComponent"));
	GetMapItemCollisionComponent->SetupAttachment(RootComponent);
	GetMapItemCollisionComponent->SetRelativeLocation(FVector(60.0, 0.0, -5.0f));
	GetMapItemCollisionComponent->SetBoxExtent(FVector(60.0f, 30.0f, 100.0f));
	GetMapItemCollisionComponent->SetCollisionProfileName(FName("MapItemSearch"));

	// Inventory			// => ���� ���� �ʿ� (24.08.06 ������)
	for (size_t i = 0; i < 4; i++)
	{
		FFPVItemInformation NewSlot;
		ItemSlot.Push(NewSlot);
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

void ATestFPVCharacter::HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision) // => ���� ĳ���Ϳ� ����.
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

// ��Ƽ���� ȣ�� �Լ�.
void ATestFPVCharacter::ChangeHandAttackCollisionProfile(FName _Name) // => ���� ����.
{
	HandAttackComponent->SetCollisionProfileName(_Name);
}

// ���� �÷��̾� �߰� �ʿ� �ڵ� (��ȯ) 07/24 => ���� ����.
void ATestFPVCharacter::PostInitializeComponents()
{
	if (GetWorld()->WorldType == EWorldType::Game
		|| GetWorld()->WorldType == EWorldType::PIE)
	{
		UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
		if (nullptr == MainGameInst)
		{
			return;
		}

		// FName�� �������� ����� �ʿ��ϴ�.
		//FName PlayerID = MainGameInst->GetUIToSelectCharacter();

		// ���̷�Ż �޽� ����
		USkeletalMesh* PlayerSkeletalMesh = MainGameInst->GetPlayerData(FName("TestPlayer"))->GetPlayerSkeletalMesh();
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh);

		// �� �޽� ���� (���� �߰� �ʿ�)
		USkeletalMesh* FPVSkeletalMesh = MainGameInst->GetPlayerData(FName("TestPlayer"))->GetPlayerFPVPlayerSkeletalMesh();
		FPVMesh->SetSkeletalMesh(FPVSkeletalMesh);

		// ABP ����
		UClass* AnimInst = Cast<UClass>(MainGameInst->GetPlayerData(FName("TestPlayer"))->GetPlayerAnimInstance());
		GetMesh()->SetAnimInstanceClass(AnimInst);
	}

	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void ATestFPVCharacter::BeginPlay()	// => ���� ���� �ʿ� (24.08.01 ����, �߰��� ��� ����)
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
	GetMapItemCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ATestFPVCharacter::MapItemOverlapStart);
	GetMapItemCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ATestFPVCharacter::MapItemOverlapEnd);


	//double MyVelocity = UKismetMathLibrary::VSizeXY(GetVelocity());
	//UCharacterMovementComponent* MyMovementComponent = GetCharacterMovement();
	//MyMovementComponent->IsFalling();
}

// Called every frame
void ATestFPVCharacter::Tick(float DeltaTime)
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

	// Debugging
	//DefaultRayCast(DeltaTime);
	//float ts = GetWorld()->GetDeltaSeconds();
	AGameModeBase* Ptr = GetWorld()->GetAuthGameMode();
	TArray<FFPVItemInformation> I = ItemSlot;
	int c = CurItemIndex;
	int a = 0;
}

void ATestFPVCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// AnimInstance
	DOREPLIFETIME(ATestFPVCharacter, LowerStateValue);	// => ���� ����.
	DOREPLIFETIME(ATestFPVCharacter, DirValue);			// => ���� ����.
	DOREPLIFETIME(ATestFPVCharacter, IsFaint);			// 7/26 �߰�
	DOREPLIFETIME(ATestFPVCharacter, IsBombSetting);	// => ���ο� ���� �ʿ� (24.07.29 �߰���)

	// Inventory
	//DOREPLIFETIME(ATestFPVCharacter, ItemSlot);		// => ���� ���� �ʿ� (24.08.06 �߰���)
	//DOREPLIFETIME(ATestFPVCharacter, IsItemIn);		// => ���� ���� �ʿ� (24.08.06 �߰���)

	// Item
	//DOREPLIFETIME(ATestFPVCharacter, RayCastToItemName); // ��� ����.

	// Server?
	DOREPLIFETIME(ATestFPVCharacter, Token); // => ���� ����.
}

void ATestFPVCharacter::FireRayCast_Implementation() // => ���� ���� �ʿ� (24.07.30 DebugMessage �κ� ������)
{
	if (CurItemIndex == -1 || CurItemIndex == 2)
	{
		return;
	}

	if (ItemSlot[CurItemIndex].ReloadLeftNum <= 0)
	{
		ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
	}

	ATestFPVPlayerController* Con = Cast<ATestFPVPlayerController>(GetController());
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

void ATestFPVCharacter::Drink()
{
	ChangeMontage(EPlayerUpperState::Drink);

	// ���� �������� ���ٸ� return
//	if (false == IsItemIn[3])
//	{
//#ifdef WITH_EDITOR
//		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), 4));
//#endif
//		return;
//	}

	// �ִϸ��̼� ����
	//ChangePosture(EPlayerPosture::Drink);

//	// ���� ������ ����
//	DeleteItem(3);
//
//	// �������� �÷��̾� HP ȸ��
//	ATestPlayerState* MyTestPlayerState = GetPlayerState<ATestPlayerState>();
//	if (nullptr == MyTestPlayerState)
//	{
//		return;
//	}
//	MyTestPlayerState->HealHp();
//
//#ifdef WITH_EDITOR
//	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, FString(TEXT("HP recovered!")));
//#endif
}

// ��Ƽ����
void ATestFPVCharacter::DrinkComplete_Implementation()			// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	ChangeMontage(IdleDefault);
}

void ATestFPVCharacter::BombSetStart_Implementation()			// => ���� ���� �ʿ� (24.08.06 �κ��丮�� �ִ��� �˻��ϴ� �κ� ������)
{
	// ��ź �������� ���ٸ� return
	if (false == ItemSlot[static_cast<int>(EItemType::Bomb)].IsItemIn)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), 4));
#endif
		return;
	}

	// ��ź ��ġ ������ Area�� �ƴ� ��� return
	AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);	// 240801 AreaObject �߰��� �ش� Ŭ���� ����
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
	//ChangePosture(EPlayerPosture::Bomb);
	ChangeMontage(EPlayerUpperState::Bomb);
}

void ATestFPVCharacter::BombSetTick_Implementation()		// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	if (true == IsBombSetting)
	{
		AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);	// 240801 AreaObject �߰��� �ش� Ŭ���� ����
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

void ATestFPVCharacter::BombSetCancel_Implementation()		// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	if (true == IsBombSetting)
	{
		// ��ź ��ġ �ߴ�
		IsBombSetting = false;
		AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);	// 240801 AreaObject �߰��� �ش� Ŭ���� ����
		if (nullptr != AreaObject)
		{
			AreaObject->ResetBombTime();
		}

		// ���� �ڼ��� �ִϸ��̼� ����
		//ChangePosture(PrevPostureValue);
		ChangeMontage(IdleDefault);
	}
}

void ATestFPVCharacter::BombSetComplete_Implementation()	// => ���ο� ���� �ʿ� (24.08.01 ������)
{
	// ��ź ��ġ �Ϸ�
	IsBombSetting = false;

	AAreaObject* AreaObject = Cast<AAreaObject>(GetMapItemData);	// 24-08-01 AreaObject �߰��� �ش� Ŭ���� ����
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
	//ChangePosture(PrevPostureValue);
	ChangeMontage(IdleDefault);
}

void ATestFPVCharacter::ChangeMontage_Implementation(EPlayerUpperState _UpperState)
{
	PlayerAnimInst->ChangeAnimation(_UpperState);
	FPVPlayerAnimInst->ChangeAnimation(_UpperState);
	ClientChangeMontage(_UpperState);
}

void ATestFPVCharacter::ClientChangeMontage_Implementation(EPlayerUpperState _UpperState)
{
	PlayerAnimInst->ChangeAnimation(_UpperState);
	FPVPlayerAnimInst->ChangeAnimation(_UpperState);
}

void ATestFPVCharacter::SetStaticMesh_Implementation(FName _ItemName)
{
	// test
	{
		UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
		const FItemDataRow* ItemData = Inst->GetItemData(_ItemName);

		int ItemReloadNum = ItemData->GetReloadNum();
		int ItemDamage = ItemData->GetDamage();
		UStaticMesh* ItemMeshRes = ItemData->GetResMesh();
		FVector ItemRelLoc = ItemData->GetRelLoc();
		FRotator ItemRelRot = ItemData->GetRelRot();
		FVector ItemRelScale = ItemData->GetRelScale();

		if (_ItemName == "SniperRifle")
		{
			ItemSlot[0].Name = _ItemName;
			ItemSlot[0].ReloadMaxNum = ItemReloadNum;
			ItemSlot[0].ReloadLeftNum = ItemReloadNum;
			ItemSlot[0].Damage = ItemDamage;
			ItemSlot[0].MeshRes = ItemMeshRes;
			ItemSlot[0].RelLoc = ItemRelLoc;
			ItemSlot[0].RelRot = ItemRelRot;
			ItemSlot[0].RelScale = ItemRelScale;
		}
		else if (_ItemName == "Katana")
		{
			ItemSlot[1].Name = _ItemName;
			ItemSlot[1].ReloadMaxNum = ItemReloadNum;
			ItemSlot[1].ReloadLeftNum = ItemReloadNum;
			ItemSlot[1].Damage = ItemDamage;
			ItemSlot[1].MeshRes = ItemMeshRes;
			ItemSlot[1].RelLoc = ItemRelLoc;
			ItemSlot[1].RelRot = ItemRelRot;
			ItemSlot[1].RelScale = ItemRelScale;
		}
	}

	//������ static mesh ����
	ItemSocketMesh->SetStaticMesh(ItemSlot[0].MeshRes);
	FPVItemSocketMesh->SetStaticMesh(ItemSlot[0].MeshRes);

	// ������ �޽� transform ����
	ItemSocketMesh->SetRelativeLocation(ItemSlot[0].RelLoc);
	FPVItemSocketMesh->SetRelativeLocation(ItemSlot[0].RelLoc);

	ItemSocketMesh->SetRelativeRotation(ItemSlot[0].RelRot);
	FPVItemSocketMesh->SetRelativeRotation(ItemSlot[0].RelRot);

	ItemSocketMesh->SetRelativeScale3D(ItemSlot[0].RelScale);
	FPVItemSocketMesh->SetRelativeScale3D(ItemSlot[0].RelScale);

	// ������ �޽� visibility �ѱ�
	ItemSocketMesh->SetVisibility(true);
	FPVItemSocketMesh->SetVisibility(true);
}

//void ATestFPVCharacter::ChangePosture_Implementation(EPlayerPosture _Type)	// => �������� �����ؾ� �� (24.07.30 ���� ��)
//{
//	PostureValue = _Type;
//	if (_Type == EPlayerPosture::Barehand)
//	{
//		// 1. Barehand => �Ǽ� �ڼ��� ����
//		
//		// ���� PostureValue�� ����.
//		PrevPostureValue = PostureValue;
//
//		// Anim Instance �� ����.
//		PostureValue = _Type;
//		
//		// == ������ ����. ==
//		CurItemIndex = 5;
//
//		// ������ �޽� visibility ����
//		ItemSocketMesh->SetVisibility(false);
//		FPVItemSocketMesh->SetVisibility(false);
//	}
//	else
//	{
//		// 2. Rifle1, Rifle2, Melee, Drink, Bomb => �������� �� �ڼ��� ����
//		int ItemSlotIndex = static_cast<int>(_Type);
//		if (IsItemIn[ItemSlotIndex] == false)
//		{
//#ifdef WITH_EDITOR
//			GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString::Printf(TEXT("The item slot is empty. (Index : %d)"), ItemSlotIndex + 1));
//#endif // WITH_EDITOR
//			return;
//		}
//		// ���� PostureValue�� ����.
//		PrevPostureValue = PostureValue;
//
//		// Anim Instance �� ����.
//		PostureValue = _Type;
//		
//		// == ������ ����. ==
//		CurItemIndex = ItemSlotIndex;
//
//		// ������ static mesh ����
//		ItemSocketMesh->SetStaticMesh(ItemSlot[CurItemIndex].MeshRes);
//		FPVItemSocketMesh->SetStaticMesh(ItemSlot[CurItemIndex].MeshRes);
//
//		// ������ �޽� transform ����
//		ItemSocketMesh->SetRelativeLocation(ItemSlot[CurItemIndex].RelLoc);
//		FPVItemSocketMesh->SetRelativeLocation(ItemSlot[CurItemIndex].RelLoc);
//
//		ItemSocketMesh->SetRelativeRotation(ItemSlot[CurItemIndex].RelRot);
//		FPVItemSocketMesh->SetRelativeRotation(ItemSlot[CurItemIndex].RelRot);
//
//		ItemSocketMesh->SetRelativeScale3D(ItemSlot[CurItemIndex].RelScale);
//		FPVItemSocketMesh->SetRelativeScale3D(ItemSlot[CurItemIndex].RelScale);
//
//		// ������ �޽� visibility �ѱ�
//		ItemSocketMesh->SetVisibility(true);
//		FPVItemSocketMesh->SetVisibility(true);
//	}
//}

void ATestFPVCharacter::CrouchCameraMove() // => ���ο� ���� �ʿ� (24.07.29 ������) => ���� ����.
{
	// Controller�� Crouch �Լ����� ȣ��.
	//if (IsFPV)
	if(PointOfView == EPlayerFPSTPSState::FPS)
	{
		switch (LowerStateValue)
		{
		case EPlayerLowerState::Idle:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc_Crouch);
			ChangeLowerState(EPlayerLowerState::Crouch);
			break;
		case EPlayerLowerState::Crouch:
			SpringArmComponent->SetRelativeLocation(FPVCameraRelLoc);
			ChangeLowerState(EPlayerLowerState::Idle);
			break;
		default:
			break;
		}
	}
	else if(PointOfView == EPlayerFPSTPSState::TPS)
	{
		switch (LowerStateValue)
		{
		case EPlayerLowerState::Idle:
			ChangeLowerState(EPlayerLowerState::Crouch);
			break;
		case EPlayerLowerState::Crouch:
			ChangeLowerState(EPlayerLowerState::Idle);
			break;
		default:
			break;
		}
	}
}

void ATestFPVCharacter::ChangeLowerState_Implementation(EPlayerLowerState _LowerState) // => ���� ����.
{
	// L-Ctrl
	LowerStateValue = _LowerState;
}

void ATestFPVCharacter::ChangePlayerDir_Implementation(EPlayerMoveDir _Dir) // => ���� ����.
{
	// W A S D
	DirValue = _Dir;
}

void ATestFPVCharacter::ChangeIsFaint_Implementation()
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

void ATestFPVCharacter::CheckItem()	// => ���� ���� �ʿ� (24.08.02 PickUpItem ���� �߰���)
{
	// �ʿ� �������� ���� ���.
	if (nullptr == GetMapItemData)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There is no item to check.")));
#endif // WITH_EDITOR
		return;
	}

	AMapObjectBase* GetMapObject = Cast<AMapObjectBase>(GetMapItemData);
	if (nullptr != GetMapObject)
	{
		// 1. �ʿ�����Ʈ�� ���
		InteractObject(GetMapObject);
	}
	else
	{
		// 2. �ֿ� �� �ִ� �������� ���
		AItemBase* GetMapItem = Cast<AItemBase>(GetMapItemData);
		PickUpItem(GetMapItem);
	}
}

void ATestFPVCharacter::InteractObject_Implementation(AMapObjectBase* _MapObject)	// => ���� ���� �ʿ� (24.08.06 Bomb �κ� ������)
{
	// Door�� ��� : ��ȣ�ۿ��� Switch�� �ߵ���Ű�Ƿ� return
	ADoorObject* DoorObject = Cast<ADoorObject>(_MapObject);
	if (nullptr != DoorObject)
	{
		return;
	}

	// Area�� ��� : ��ȣ�ۿ��� �÷��̾��ʿ��� ó���ؾ� �ϹǷ� return	// 240801 AreaObject �߰��� �ش� ���� �߰��Ǿ����ϴ�
	AAreaObject* AreaObject = Cast<AAreaObject>(_MapObject);
	if (nullptr != AreaObject)
	{
		return;
	}

	// �� �� �ʿ�����Ʈ(Switch ��)�� ��� : ��ȣ�ۿ� �ߵ�
	_MapObject->InterAction();
}

void ATestFPVCharacter::PickUpItem(AItemBase* _Item)	// => ���� ���� �ʿ� (24.08.06 ������)
{
	const FItemDataRow* ItemData = _Item->GetItemData();

	EItemType ItemType = ItemData->GetItemType();
	int ItemSlotIndex = static_cast<int>(ItemType);
	FName ItemName = *(ItemData->GetName());

	// �̹� ���� �̸��� ���� �������� �κ��丮�� ���� ��� return
	if (ItemName == ItemSlot[ItemSlotIndex].Name)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("The same item is already in inventory. (Index : %d)"), ItemSlotIndex + 1));
#endif // WITH_EDITOR
		return;
	}

	// �̹� ���� Ÿ���� �������� �κ��丮�� ���� ��� ������ �ִ� �������� Drop
	if (true == ItemSlot[ItemSlotIndex].IsItemIn)
	{
		DropItem(ItemSlotIndex);
	}

	// �κ��丮�� PickUp�� ������ ���� �ֱ�
	ItemSlot[ItemSlotIndex].IsItemIn = true;								
	ItemSlot[ItemSlotIndex].Name = ItemName;								
	ItemSlot[ItemSlotIndex].ReloadMaxNum = ItemData->GetReloadNum();		// ���� ���� ����	 (Max) (-1�� ��� �ѱ�� ���Ⱑ �ƴ�)
	ItemSlot[ItemSlotIndex].ReloadLeftNum = ItemData->GetReloadNum();		// ���� ���� ����	 (Left) (-1�� ��� �ѱ�� ���Ⱑ �ƴ�)
	ItemSlot[ItemSlotIndex].Damage = ItemData->GetDamage();					// ���� ���ݷ� (0�� ��� ���Ⱑ �ƴ�)
	ItemSlot[ItemSlotIndex].MeshRes = ItemData->GetResMesh();				// ����ƽ �޽� ���ҽ�
	ItemSlot[ItemSlotIndex].RelLoc = ItemData->GetRelLoc();					// ItemSocket, FPVItemSocket ����� ��ġ
	ItemSlot[ItemSlotIndex].RelRot = ItemData->GetRelRot();					// ItemSocket, FPVItemSocket ����� ȸ��
	ItemSlot[ItemSlotIndex].RelScale = ItemData->GetRelScale();				// ItemSocket, FPVItemSocket ����� ũ��

#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Picked up new item! (Index : %d)"), ItemSlotIndex + 1));
#endif // WITH_EDITOR

	// �ʵ忡 �����ϴ� ������ ���� ����
	DestroyItem(_Item);

	// �ִϸ��̼� ������Ʈ
	ItemToCheckAnimation();
}

void ATestFPVCharacter::DropItem_Implementation(int _SlotIndex)	// => ���� ���� �ʿ� (24.08.06 ������)
{
	// DropItem �� �� ���� ��� 1 : �Ǽ��� ��
	if (CurItemIndex == -1)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There's no item to drop. (Current posture is 'Barehand')")));
#endif // WITH_EDITOR
		return;
	}

	// DropItem �� �� ���� ��� 2 : (�׷����� ��������) ���� Posture�� �ش��ϴ� ���Ⱑ �κ��丮�� ���� ��
	if (false == ItemSlot[CurItemIndex].IsItemIn)
	{
#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Yellow, FString(TEXT("There's no item to drop. (The item slot is empty)")));
#endif // WITH_EDITOR
		return;
	}

	// ����Ʈ�� �������� Actor�� ����
	UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());	// ���ο��� �̷��� �����.
	//UMainGameInstance* MainGameInst = GetWorld()->GetGameInstanceChecked<UMainGameInstance>();
	const FItemDataRow* ItemData = MainGameInst->GetItemData(ItemSlot[_SlotIndex].Name);
	FTransform SpawnTrans = GetActorTransform();
	SpawnTrans.SetTranslation(GetActorLocation() + (GetActorForwardVector() * 100.0f) + (GetActorUpVector() * 50.0f));

	// Spawn
	AActor* DropItem = GetWorld()->SpawnActor<AActor>(ItemData->GetItemUClass(), SpawnTrans);
	UStaticMeshComponent* DropItemMeshComp = Cast<AItemBase>(DropItem)->GetStaticMeshComponent();
	DropItemMeshComp->SetSimulatePhysics(true);
	//bool r = DropItemMeshComp->IsSimulatingPhysics();
	//int a = 0;

	// ������ ������ ������ (�̿�)
	//GetMesh()->SetSimulatePhysics(true);
	FVector ImpulseVector = GetActorForwardVector() * 1000.0f;
	GetMesh()->AddImpulse(ImpulseVector, FName("RightHand"), false);

	// �κ��丮���� ���� ������ ���� ����
	DeleteItem(_SlotIndex);

#ifdef WITH_EDITOR
	GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("Drop the item. (Index : %d)"), _SlotIndex + 1));
#endif // WITH_EDITOR

	// �ִϸ��̼� ������Ʈ
	//ChangePosture(EPlayerPosture::Barehand);
	ChangeMontage(EPlayerUpperState::UArm_Attack);
}

void ATestFPVCharacter::DestroyItem_Implementation(AItemBase* _Item)
{
	_Item->Destroy();
}

void ATestFPVCharacter::DeleteItem(int _Index)		// => ���� ���� �ʿ� (24.08.06 ������)
{
	FFPVItemInformation NewSlot;
	ItemSlot[_Index] = NewSlot;
}

void ATestFPVCharacter::ChangePOV()	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 ���� ��) => ���� ����.
{
	//if (IsFPV)	// ����Ī -> ����Ī
	if (PointOfView == EPlayerFPSTPSState::FPS)
	{
		// SpringArm Component ��ġ ����.
		//SpringArmComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SpringArmComponent->TargetArmLength = 300.0f;
		SpringArmComponent->SetRelativeLocation(CameraRelLoc);

		// Character Mesh ��ȯ.
		GetMesh()->SetOwnerNoSee(false);
		FPVMesh->SetOwnerNoSee(true);

		// Item Mesh
		for (int i = 0; i < static_cast<int>(EPlayerUpperState::UArm_Attack); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(false);
			FPVItemSocketMesh->SetOwnerNoSee(true);
		}

		//IsFPV = false;
		PointOfView = EPlayerFPSTPSState::TPS;
	}
	else if(PointOfView == EPlayerFPSTPSState::TPS)	// ����Ī -> ����Ī
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
		for (int i = 0; i < static_cast<int>(EPlayerUpperState::UArm_Attack); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(true);
			FPVItemSocketMesh->SetOwnerNoSee(false);
		}

		//IsFPV = true;
		PointOfView = EPlayerFPSTPSState::FPS;
	}
}

void ATestFPVCharacter::CharacterReload() // => ���� ����.
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

void ATestFPVCharacter::MapItemOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)	// => ���� ���� �ʿ� (24.08.02 AreaObject�� ��� ���� �˻� �κ� ����)
{
	GetMapItemData = OtherActor;

	ATestFPVPlayerController* MyController = Cast<ATestFPVPlayerController>(GetController());
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

void ATestFPVCharacter::MapItemOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)	// => ���� ���� �ʿ� (24.08.01 ������)
{
	if (nullptr != GetMapItemData)
	{
		GetMapItemData = nullptr;
	}

	ATestFPVPlayerController* MyController = Cast<ATestFPVPlayerController>(GetController());
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


void ATestFPVCharacter::AttackCheck()
{
	// Mouse Left Button 

	switch (IdleDefault)
	{
	case EPlayerUpperState::UArm_Idle:
		ChangeMontage(EPlayerUpperState::UArm_Attack);
		break;
	case EPlayerUpperState::Rifle_Idle:
		ChangeMontage(EPlayerUpperState::Rifle_Attack);
		break;
	case EPlayerUpperState::Melee_Idle:
		ChangeMontage(EPlayerUpperState::Melee_Attack);
		break;
	default:
		break;
	}
}

void ATestFPVCharacter::AttackEndCheck()
{
	UAnimMontage* GetCurMontage = GetCurrentMontage();
	FName GetCurMontageName = GetCurMontage->GetFName();
	// E_Fire_Stand_Montage
	// E_BareHand_Attack_Montage

	if ("E_Fire_Stand_Montage" == GetCurMontageName || "E_BareHand_Attack_Montage" == GetCurMontageName)
	{
		// ��Ƽ���� ��ȣ ����?
		return;
	}
	else
	{
		ChangeMontage(IdleDefault);
	}
}

void ATestFPVCharacter::NetCheck() // => ���� ����.
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

void ATestFPVCharacter::SendTokenToHpBarWidget() // => ���� ���� ���� ��.(HUD, Widget �����.)
{
	ATestFPVPlayerController* Con = Cast<ATestFPVPlayerController>(GetController());
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

void ATestFPVCharacter::UpdatePlayerHp(float _DeltaTime) // => ���� ���� ���� ��.(HUD, Widget �����.)
{
	ATestPlayerState* MyTestPlayerState = GetPlayerState<ATestPlayerState>();
	if (nullptr == MyTestPlayerState)
	{
		return;
	}

	ATestFPVPlayerController* MyController = Cast<ATestFPVPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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

void ATestFPVCharacter::SettingPlayerState_Implementation()
{
	ATestFPVPlayerController* Con = Cast<ATestFPVPlayerController>(GetController());
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

void ATestFPVCharacter::ShowMuzzle()
{
	FTransform CreatePosition = GetMesh()->GetBoneTransform(FName("RightHand"), ERelativeTransformSpace::RTS_World);
	GetWorld()->SpawnActor<AActor>(AMuzzleParticleActor::StaticClass(), CreatePosition);
}

void ATestFPVCharacter::ItemToCheckAnimation()
{

}