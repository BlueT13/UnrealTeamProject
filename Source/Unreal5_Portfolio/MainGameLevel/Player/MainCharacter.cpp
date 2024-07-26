// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/Player/MainCharacter.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Global/MainGameInstance.h"
#include "Global/DataTable/ItemDataRow.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "MainPlayerController.h"
#include "PlayerItemInformation.h"
#include "PartDevLevel/Character/PlayerAnimInstance.h"
#include "Components/SphereComponent.h"
#include "MainGameLevel/Player/MainPlayerState.h"
#include "MainGameLevel/Monster/Base/BasicMonsterBase.h"
#include "PartDevLevel/Monster/Boss/TestBossMonsterBase.h"
#include "MainGameLevel/Object/MapObjectBase.h"
#include "MainGameLevel/Object/Bomb.h"
#include "MainGameLevel/UI/Title/MainTitleHUD.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Init

	// SpringArm Component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetRelativeLocation(FVector(20.0f, 0.0f, 67.0f));
	SpringArmComponent->TargetArmLength = 0.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->bDoCollisionTest = true;

	// Camera Component
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetProjectionMode(ECameraProjectionMode::Perspective);

	// MinimapIcon Component
	//MinimapIconComponent = CreateDefaultSubobject<UTestMinimapIconComponent>(TEXT("MinimapPlayerIcon"));
	//MinimapIconComponent->SetupAttachment(RootComponent);
	//MinimapIconComponent->bVisibleInSceneCaptureOnly = true;
	
	// character Mesh
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bHiddenInSceneCapture = true;

	// FPV Character Mesh
	FPVMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	FPVMesh->SetupAttachment(CameraComponent);
	FPVMesh->SetOnlyOwnerSee(true);
	FPVMesh->bCastDynamicShadow = false;
	FPVMesh->CastShadow = false;

	// Riding Character Mesh
	//RidingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RidingMesh"));
	//RidingMesh->SetupAttachment(GetMesh());
	//RidingMesh->SetCollisionProfileName(TEXT("NoCollision"));
	//RidingMesh->SetVisibility(false);
	//RidingMesh->SetIsReplicated(true);
	//RidingMesh->bHiddenInSceneCapture = true;

	// ������ ���� ���� �ʱ�ȭ.
	ItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemSocketMesh"));
	ItemSocketMesh->SetupAttachment(GetMesh(), FName("ItemSocket"));
	ItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	ItemSocketMesh->SetGenerateOverlapEvents(true);
	ItemSocketMesh->SetOwnerNoSee(true);
	ItemSocketMesh->SetVisibility(false);
	ItemSocketMesh->SetIsReplicated(true);
	ItemSocketMesh->bHiddenInSceneCapture = true;

	// 1��Ī ������ ���� ���� �ʱ�ȭ.
	FPVItemSocketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FPVItemSocketMesh"));
	FPVItemSocketMesh->SetupAttachment(FPVMesh, FName("FPVItemSocket"));
	FPVItemSocketMesh->SetCollisionProfileName(TEXT("NoCollision"));
	FPVItemSocketMesh->SetGenerateOverlapEvents(true);
	FPVItemSocketMesh->SetOnlyOwnerSee(true);
	FPVItemSocketMesh->SetVisibility(false);
	FPVItemSocketMesh->SetIsReplicated(true);
	FPVItemSocketMesh->bCastDynamicShadow = false;
	FPVItemSocketMesh->CastShadow = false;

	// Item Create Component
	CreateItemComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CreateItemComponent"));
	CreateItemComponent->SetupAttachment(RootComponent);
	CreateItemComponent->SetRelativeLocation(FVector(100.0, 0.0, -90.0));

	// Map Item 
	GetMapItemCollisonComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("GetMapItemCollisionComponent"));
	GetMapItemCollisonComponent->SetupAttachment(RootComponent);
	GetMapItemCollisonComponent->SetRelativeLocation(FVector(60.0, 0.0, -5.0f));
	GetMapItemCollisonComponent->SetCollisionProfileName(FName("MapItemSearch"));
	GetMapItemCollisonComponent->SetBoxExtent(FVector(40.0, 50.0, 80.0));

	// Inventory
	for (size_t i = 0; i < static_cast<size_t>(EPlayerPosture::Barehand); i++)
	{
		FPlayerItemInformation SlotBase;
		SlotBase.Name = "";
		SlotBase.ReloadMaxNum = -1;
		SlotBase.ReloadLeftNum = -1;
		ItemSlot.Push(SlotBase);
		IsItemIn.Push(false);
	}

	// Hand Attack Component
	HandAttackComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Hand Attack Comp"));
	HandAttackComponent->SetupAttachment(GetMesh());
	HandAttackComponent->SetRelativeLocation({ 0.0f, 100.0f, 110.0f });
}

void AMainCharacter::PostInitializeComponents() // FName �κ� ���� �ʿ�.
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
		USkeletalMesh* PlayerSkeletalMesh = MainGameInst->GetPlayerData(FName("AlienSoldier"))->GetPlayerSkeletalMesh();
		GetMesh()->SetSkeletalMesh(PlayerSkeletalMesh);

		// ABP ����
		UClass* AnimInst = Cast<UClass>(MainGameInst->GetPlayerData(FName("AlienSoldier"))->GetPlayerAnimInstance());
		GetMesh()->SetAnimInstanceClass(AnimInst);
	}

	Super::PostInitializeComponents();
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	NetCheck();
	Super::BeginPlay();
	UMainGameBlueprintFunctionLibrary::PushActor(EObjectType::Player, this);

	PlayerAnimInst = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	FPVPlayerAnimInst = Cast<UPlayerAnimInstance>(FPVMesh->GetAnimInstance());

	HandAttackComponent->SetCollisionProfileName(TEXT("NoCollision"));
}

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// ��ü ����
	DOREPLIFETIME(AMainCharacter, LowerStateValue);
	// �÷��̾� �ڼ� ����.
	DOREPLIFETIME(AMainCharacter, PostureValue);
	DOREPLIFETIME(AMainCharacter, DirValue);

	DOREPLIFETIME(AMainCharacter, Token);
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePlayerHp(DeltaTime);
}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainCharacter::ChangePosture_Implementation(EPlayerPosture _Type)
{
	// Bomb, Drink ������ ��� �ڼ��� ������ �� ������ ����.
	if (_Type == EPlayerPosture::Bomb || _Type == EPlayerPosture::Drink)
	{
		return;
	}
	else if (_Type == EPlayerPosture::Barehand) // �Ǽ� �ڼ��� ����.
	{
		PostureValue = _Type;
		CurItemIndex = -1;

		ItemSocketMesh->SetVisibility(false);
		FPVItemSocketMesh->SetVisibility(false);
	}
	else // ���⸦ �� �ڼ��� ����.
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

void AMainCharacter::ChangeLowerState_Implementation(EPlayerLowerState _LowerState)
{
	LowerStateValue = _LowerState;
}

void AMainCharacter::ChangePlayerDir_Implementation(EPlayerMoveDir _Dir)
{
	DirValue = _Dir;
}

void AMainCharacter::PickUpItem_Implementation()
{
	// �ʿ� �������� ���� ���. -> �ݸ����� �浹�� ���� ���
	if (nullptr == GetMapItemData)
	{
		return;
	}

	// 1. �ʿ�����Ʈ�� ���
	{
		AMapObjectBase* GetMapItem = Cast<AMapObjectBase>(GetMapItemData);
		if (nullptr != GetMapItem)
		{
			GetMapItem->InterAction();

			ABomb* GetSampleData = Cast<ABomb>(GetMapItem);
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
			return;
		}
	}

	// 2. �ֿ� �� �ִ� �������� ���
	// �ʿ� Actor�� �ִٸ� �ش� Actor�� Tag�� �����´�.
	FString TagName = "";
	for (size_t i = 0; i < GetMapItemData->Tags.Num(); i++) // �Ҿ��� ��.
	{
		TagName = GetMapItemData->Tags[i].ToString();
	}

	FName ItemStringToName = FName(*TagName);

	// ItemName�� �´� ������ ������ DT���� �����´�.
	UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
	const FItemDataRow* ItemData = Inst->GetItemData(ItemStringToName);

	EPlayerPosture ItemType = ItemData->GetType();		// ������ Ÿ��

	// �̹� �κ��丮�� ���� �̸��� ���� �������� ���� ���.
	if (ItemStringToName == ItemSlot[static_cast<int>(ItemType)].Name)
	{
		return;
	}

	// �̹� �κ��丮�� ���� Ÿ���� �������� ���� ���. (���� ������ ���� ����)
	if (true == IsItemIn[static_cast<uint8>(ItemType)])
	{
		CharacterPlayerToDropItem();
	}

	int ItemReloadNum = ItemData->GetReloadNum();		// ���� ���� ���� (30, 40)	// -1�� ��� �ѱ�� ���Ⱑ �ƴ�
	int ItemDamage = ItemData->GetDamage();				// ���� ���ݷ�				// 0�� ��� ���Ⱑ �ƴ�
	UStaticMesh* ItemMeshRes = ItemData->GetResMesh();	// ����ƽ �޽�
	FVector ItemRelLoc = ItemData->GetRelLoc();			// ����ƽ �޽� ������Ʈ ����� ��ġ
	FRotator ItemRelRot = ItemData->GetRelRot();		// ����ƽ �޽� ������Ʈ ����� ȸ��
	FVector ItemRelScale = ItemData->GetRelScale();		// ����ƽ �޽� ������Ʈ ����� ũ��

	uint8 ItemIndex = static_cast<uint8>(ItemType);		// �������� ���� �κ��丮 �ε���

	// �տ� �������� �����Ѵ�.
	// �κ��丮�� ������ ����ֱ�. (����ƽ�޽÷� �������� �������� ��� ä��!!!)
	IsItemIn[ItemIndex] = true;

	ItemSlot[ItemIndex].Name = ItemStringToName;
	ItemSlot[ItemIndex].ReloadMaxNum = ItemReloadNum;
	ItemSlot[ItemIndex].ReloadLeftNum = ItemReloadNum;
	ItemSlot[ItemIndex].Damage = ItemDamage;
	ItemSlot[ItemIndex].MeshRes = ItemMeshRes;
	ItemSlot[ItemIndex].RelLoc = ItemRelLoc;
	ItemSlot[ItemIndex].RelRot = ItemRelRot;
	ItemSlot[ItemIndex].RelScale = ItemRelScale;

	// ���� ���� ����.
	GetMapItemData->Destroy();

	// ���� Type�� ���� �ִϸ��̼� ��ȭ �Լ� ȣ��.
	ChangePosture(ItemType);
}

void AMainCharacter::CharacterPlayerToDropItem_Implementation()
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
	UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
	const FItemDataRow* ItemBase = MainGameInst->GetItemData(ItemName);
	FTransform BoneTrans = GetMesh()->GetBoneTransform(FName("weapon_r"), ERelativeTransformSpace::RTS_World);
	GetWorld()->SpawnActor<AActor>(ItemBase->GetItemUClass(), BoneTrans);

	// �տ� ��� �ִ� �������� �κ��丮���� ����
	FPlayerItemInformation NewSlot;
	ItemSlot[CurItemIndex] = NewSlot;
	IsItemIn[CurItemIndex] = false;

	// �ڼ��� �Ǽ����� ����
	ChangePosture(EPlayerPosture::Barehand);
}

void AMainCharacter::FireRayCast_Implementation(float _DeltaTime)
{
	if (CurItemIndex == -1 || CurItemIndex == 2 || CurItemIndex == 5)
	{
		// �ָ�, �ٰŸ�
		if (PostureValue == EPlayerPosture::Barehand || PostureValue == EPlayerPosture::Melee)
		{
			ChangeMontage();
		}
		return;
	}

	// ź���� ���ٸ� 
	if (ItemSlot[CurItemIndex].ReloadLeftNum <= 0)
	{
		//ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
		// �����϶�� Widget�� ����� ��.
		// ���� �Լ��� CharacterReload �̴�.
		return;
	}

	


	AMainPlayerController* Con = Cast<AMainPlayerController>(GetController());
	FVector Start = GetMesh()->GetSocketLocation(FName("weapon_r_muzzle"));
	Start.Z -= 20.0f;
	FVector End = (Con->GetControlRotation().Vector() * 2000.0f) + Start;
	
	FHitResult Hit;
	if (GetWorld())
	{
		// ź�� ���.
		ItemSlot[CurItemIndex].ReloadLeftNum -= 1;

		// Ray Cast
		TArray<AActor*> IgnoreActors; // ������ Actor��.
		bool ActorHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);
		
		if (true == ActorHit && nullptr != Hit.GetActor())
		{
			FString BoneName = Hit.BoneName.ToString();
			//UE_LOG(LogTemp, Warning, TEXT("Bone Name : %s"), *BoneName);
			ABasicMonsterBase* Monster = Cast<ABasicMonsterBase>(Hit.GetActor());
			if (nullptr != Monster)
			{
				Monster->Damaged(ItemSlot[CurItemIndex].Damage);
			}
		}
	}
}

void AMainCharacter::ChangeMontage_Implementation()
{
	PlayerAnimInst->ChangeAnimation(PostureValue);
	FPVPlayerAnimInst->ChangeAnimation(PostureValue);

	ClientChangeMontage();
}

void AMainCharacter::ClientChangeMontage_Implementation()
{
	PlayerAnimInst->ChangeAnimation(PostureValue);
	FPVPlayerAnimInst->ChangeAnimation(PostureValue);
}

void AMainCharacter::CrouchCameraMove()
{
	if (IsFPV)
	{
		switch (LowerStateValue)
		{
		case EPlayerLowerState::Idle:
			SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 10.0f));
			break;
		case EPlayerLowerState::Crouch:
			SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));
			break;
		default:
			break;
		}
	}
}

void AMainCharacter::MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	GetMapItemData = _OtherActor;
}

void AMainCharacter::MapItemOverlapEnd()
{
	if (nullptr != GetMapItemData)
	{
		GetMapItemData = nullptr;
	}
}

void AMainCharacter::UpdatePlayerHp(float _DeltaTime)
{
	AMainPlayerState* MyMainPlayerState = GetPlayerState<AMainPlayerState>();
	if (nullptr == MyMainPlayerState)
	{
		return;
	}

	AMainPlayerController* MyController = Cast<AMainPlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (nullptr == MyController)
	{
		return;
	}

	float GetHp = MyMainPlayerState->GetPlayerHp();

	// Get HUD
	// AMainPlayHUD PlayHUD = Cast<AMainPlayHUD>(MyController->GetHUD());
	// if(nullptr == )
	// {
	//     UHpBarUserWidget* MyHpWidget = Cast<UHpBarUserWidget>(PlayHUD->GetWidget(EInGameUIType::HpBar));
	//     MyHpWidget->NickNameUpdate(Token, FText::FromString(FString("")));
	//     MyHpWidget->HpbarUpdate(Token, CurHp, 100.0f);
	// {
}

void AMainCharacter::ChangePOV()
{
	if (IsFPV)
	{
		// SpringArm ��ġ ����
		SpringArmComponent->TargetArmLength = 200.0f;
		SpringArmComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 80.0f));

		// Character Mesh ��ȯ
		GetMesh()->SetOwnerNoSee(false);
		FPVMesh->SetOwnerNoSee(true);

		// Item Mesh
		for (int i = 0; i < int(EPlayerPosture::Barehand); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(false);
			FPVItemSocketMesh->SetOwnerNoSee(true);
		}

		// ����Ī -> ����Ī
		IsFPV = false;
	}
	else
	{
		// SpringArm ��ġ ����
		SpringArmComponent->TargetArmLength = 0.0f;
		SpringArmComponent->SetRelativeLocation(FVector(20.0f, 0.0f, 67.0f));

		// Character Mesh ��ȯ
		GetMesh()->SetOwnerNoSee(true);
		FPVMesh->SetOwnerNoSee(false);

		// Item Mesh
		for (int i = 0; i < int(EPlayerPosture::Barehand); i++)
		{
			ItemSocketMesh->SetOwnerNoSee(true);
			FPVItemSocketMesh->SetOwnerNoSee(false);
		}

		// ����Ī -> ����Ī
		IsFPV = true;
	}
}

void AMainCharacter::CharacterReload()
{
	if (-1 == CurItemIndex)
	{
		return;
	}
	ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
}

void AMainCharacter::HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	{
		ABasicMonsterBase* Monster = Cast<ABasicMonsterBase>(_OtherActor);
		if (nullptr != Monster)
		{
			Monster->Damaged(150.0f);
		}
	}

	{
		ATestBossMonsterBase* BossMonster = Cast<ATestBossMonsterBase>(_OtherActor); // ���� Main���� �ٲ�� ��.
		if (nullptr != BossMonster)
		{
			BossMonster->Damaged(150.0f);
		}
	}
}

void AMainCharacter::ChangeHandAttackCollisionProfile(FName _Name)
{
	HandAttackComponent->SetCollisionProfileName(_Name);
}

void AMainCharacter::NetCheck()
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
	}
}

void AMainCharacter::SendTokenToHpBarWidget()
{
	//AMainPlayerController* Con = Cast<AMainPlayerController>(GetController());
	//if (nullptr == Con)
	//{
	//	return;
	//}
	//AMainPlayHUD* PlayHUD = Cast<ATestPlayHUD>(Con->GetHUD());
	//if (nullptr == PlayHUD)
	//{
	//	return;
	//}
	//UTestHpBarUserWidget* MyHpWidget = Cast<UTestHpBarUserWidget>(PlayHUD->GetWidget(EUserWidgetType::HpBar));
	//if (nullptr == MyHpWidget)
	//{
	//	return;
	//}

	//if (true == IsCanControlled && -1 != Token)
	//{
	//	MyHpWidget->HpbarInit_ForMainPlayer(Token);
	//}
}

TArray<struct FPlayerItemInformation> AMainCharacter::GetItemSlot()
{
	return ItemSlot;
}
