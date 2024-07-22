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
#include "Kismet/KismetSystemLibrary.h"
#include "MainGameLevel/Monster/Base/BasicMonsterBase.h"

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

	// Character Mesh
	GetMesh()->SetOwnerNoSee(true);
	GetMesh()->bHiddenInSceneCapture = true;

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
	GetMapItemCollisonComponent->SetRelativeLocation(FVector(100.0, 0.0, -70.0f));
	GetMapItemCollisonComponent->SetCollisionProfileName(FName("MapItemSearch"));

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
	HandAttackComponent->SetRelativeLocation({ 0.0f, 80.0f, 120.0f });
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

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMainCharacter::ChangePosture_Implementation(EPlayerPosture _Type)
{
	if (_Type == EPlayerPosture::Barehand)
	{
		PostureValue = _Type;
		CurItemIndex = -1;

		ItemSocketMesh->SetVisibility(false);
		FPVItemSocketMesh->SetVisibility(false);
	}
	else
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
	// �ʿ� �������� ���� ���.
	if (nullptr == GetMapItemData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Not Item"));
		return;
	}

	// �ʿ� �������� �ִٸ� �ش� �������� Tag�� �����´�.
	FString TagName = "";
	for (size_t i = 0; i < GetMapItemData->Tags.Num(); i++)
	{
		TagName = GetMapItemData->Tags[i].ToString();
	}

	FName ItemName = FName(*TagName);

	// ItemName�� �´� ������ ������ DT���� �����´�.
	UMainGameInstance* Inst = GetGameInstance<UMainGameInstance>();
	const FItemDataRow* ItemData = Inst->GetItemData(ItemName);

	EPlayerPosture ItemType = ItemData->GetType();		// ������ Ÿ��
	int ItemReloadNum = ItemData->GetReloadNum();		// ���� ���� ���� (30, 40)	// -1�� ��� �ѱ�� ���Ⱑ �ƴ�
	int ItemDamage = ItemData->GetDamage();				// ���� ���ݷ�				// 0�� ��� ���Ⱑ �ƴ�
	UStaticMesh* ItemMeshRes = ItemData->GetResMesh();	// ����ƽ �޽�
	FVector ItemRelLoc = ItemData->GetRelLoc();			// ����ƽ �޽� ������Ʈ ����� ��ġ
	FRotator ItemRelRot = ItemData->GetRelRot();		// ����ƽ �޽� ������Ʈ ����� ȸ��

	uint8 ItemIndex = static_cast<uint8>(ItemType);		// �������� ���� �κ��丮 �ε���
	// �տ� �������� �־�?
	if (true == IsItemIn[ItemIndex])
	{
		// Map�� ������ ����.
		FTransform CreatePos = CreateItemComponent->GetComponentToWorld(); // üũ �ʿ�.
		CharacterPlayerToDropItem(ItemName, CreatePos);
	}

	// �տ� �������� �����Ѵ�.
	// �κ��丮�� ������ ����ֱ�. (����ƽ�޽÷� �������� �������� ��� ä��!!!)
	IsItemIn[ItemIndex] = true;

	ItemSlot[ItemIndex].Name = ItemName;
	ItemSlot[ItemIndex].ReloadMaxNum = ItemReloadNum;
	ItemSlot[ItemIndex].ReloadLeftNum = ItemReloadNum;
	ItemSlot[ItemIndex].Damage = ItemDamage;
	ItemSlot[ItemIndex].MeshRes = ItemMeshRes;
	ItemSlot[ItemIndex].RelLoc = ItemRelLoc;
	ItemSlot[ItemIndex].RelRot = ItemRelRot;

	// ���� ���� ����.
	GetMapItemData->Destroy();

	// ���� Type�� ���� �ִϸ��̼� ��ȭ �Լ� ȣ��.
	ChangePosture(ItemType);
}

void AMainCharacter::CharacterPlayerToDropItem_Implementation(FName _ItemName, FTransform _Transform)
{
	UMainGameInstance* MainGameInst = GetWorld()->GetGameInstanceChecked<UMainGameInstance>();
	const FItemDataRow* ItemBase = MainGameInst->GetItemData(_ItemName);
	GetWorld()->SpawnActor<AActor>(ItemBase->GetItemUClass(), _Transform);
}

void AMainCharacter::FireRayCast_Implementation(float _DeltaTime)
{
	if (CurItemIndex == -1 || ItemSlot[CurItemIndex].ReloadMaxNum == -1)
	{
		return;
	}

	if (ItemSlot[CurItemIndex].ReloadLeftNum <= 0)
	{
		ItemSlot[CurItemIndex].ReloadLeftNum = ItemSlot[CurItemIndex].ReloadMaxNum;
	}


	AMainPlayerController* Con = Cast<AMainPlayerController>(GetController());
	FVector Start = GetMesh()->GetSocketLocation(FName("weapon_r_muzzle"));
	Start.Z -= 20.0f;
	FVector End = (Con->GetControlRotation().Vector() * 2000.0f) + Start;
	
	FHitResult Hit;
	if (GetWorld())
	{
		ItemSlot[CurItemIndex].ReloadLeftNum -= 1;

		// Ray Cast
		TArray<AActor*> IgnoreActors; // ������ Actor��.
		bool ActorHit = UKismetSystemLibrary::LineTraceSingle(GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, Hit, true, FLinearColor::Red, FLinearColor::Green, 5.0f);
		
		if (true == ActorHit && nullptr != Hit.GetActor())
		{
			FString BoneName = Hit.BoneName.ToString();
			UE_LOG(LogTemp, Warning, TEXT("Bone Name : %s"), *BoneName);
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

void AMainCharacter::MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	GetMapItemData = _OtherActor;
}

void AMainCharacter::MapItemOverlapEnd()
{
	if (nullptr != GetMapItemData)
	{
		// ���� ����
		//GetMapItemData = nullptr;
	}
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

void AMainCharacter::HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	ABasicMonsterBase* Monster = Cast<ABasicMonsterBase>(_OtherActor);
	if (nullptr == Monster)
	{
		return;
	}

	Monster->Damaged(150.0f);
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
