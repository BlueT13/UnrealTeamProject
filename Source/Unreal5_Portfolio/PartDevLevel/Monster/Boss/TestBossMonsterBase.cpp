// Fill out your copyright notice in the Description page of Project Settings.


#include "PartDevLevel/Monster/Boss/TestBossMonsterBase.h"
#include "PartDevLevel/Monster/Boss/BossProjectile.h"
#include "TestBossMonsterAIControllerBase.h"

#include "TestLevel/Character/TestPlayerState.h"
#include "TestLevel/Character/TestCharacter.h"
#include "TestLevel/UI/TestPlayHUD.h"

#include "MainGameLevel/UI/InGame/BossHpbarUserWidget.h"
#include "MainGameLevel/Object/ReportObject.h"

#include "GameFrameWork/CharacterMovementComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/ContentsEnum.h"
#include "Global/ContentsLog.h"
#include "Global/Animation/MainAnimInstance.h"
#include "Global/MainGameState.h"

#include "Components/SphereComponent.h"
#include "BrainComponent.h"

// Sets default values
ATestBossMonsterBase::ATestBossMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeleeAttackComponent = CreateDefaultSubobject<USphereComponent>(TEXT("MeleeAttack Component"));
	MeleeAttackComponent->SetupAttachment(RootComponent);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

}

// Called when the game starts or when spawned
void ATestBossMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());

	UAnimInstance* AnimInst = GetMesh()->GetAnimInstance();
	MainAnimInst = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	//  ���� ������ ����
	SettingBossData = NewObject<UBossData>(this);
	SettingBossData->Data = MainGameInst->GetBossDataTable(BossDataName);
	float MaxHp = SettingBossData->Data->GetHP();
	SettingBossData->HP = MaxHp;
	
	//Test
	Hp = MaxHp;
	CurHp = Hp;

	if (nullptr == SettingBossData->Data)
	{
		LOG(MonsterLog, Fatal, TEXT("BossData Is Null"));
		return;
	}

	// ��Ÿ�� Ǫ��
	TMap<EBossMonsterAnim, UAnimMontage*> AnimMontages = SettingBossData->Data->GetAnimMontage();
	for (TPair<EBossMonsterAnim, class UAnimMontage*> Montage : AnimMontages)
	{
		MainAnimInst->PushAnimation(Montage.Key, Montage.Value);
	}

	// ���� ü�� UHD
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (nullptr == PlayerController)
	{
		LOG(MonsterLog, Fatal, "PlayerController is null");
	}

	ATestPlayHUD* BossUHD = Cast<ATestPlayHUD>(PlayerController->GetHUD());
	if (nullptr == BossUHD)
	{
		LOG(MonsterLog, Fatal, "BossHUD is Bullptr");
		return;
	}

	Cast<UBossHpbarUserWidget>(BossUHD->GetWidget(EUserWidgetType::BossHpbar))->SetBossName(FText::FromString(FString("Heart")));

	Cast<UBossHpbarUserWidget>(BossUHD->GetWidget(EUserWidgetType::BossHpbar))->SetHp(Hp, SettingBossData->Data->GetHP());

	BossUHD->UIOn(EUserWidgetType::BossHpbar);

	// Ŭ���̾�Ʈ�� ���
	ATestBossMonsterAIControllerBase* AIController = GetController<ATestBossMonsterAIControllerBase>();
	if (nullptr == AIController)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	if (!BlackboardComp)
	{
		UE_LOG(LogTemp, Error, TEXT("BlackboardComponent is nullptr in ATestBossMonsterBase::BeginPlay"));
		return;
	}

	// SettingBossData�� nullptr�� �ƴ��� Ȯ��
	if (!SettingBossData)
	{
		UE_LOG(LogTemp, Error, TEXT("SettingBossData is nullptr in ATestBossMonsterBase::BeginPlay"));
		return;
	}

	AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("BossMonsterData"), SettingBossData);

	MeleeAttackComponent->OnComponentEndOverlap.AddDynamic(this, &ATestBossMonsterBase::OnAttackOverlapEnd);
	SetAttackCollision(false);

	//BossUHD->GetWidget(EUserWidgetType::BossHpbar);
	//BossUHD->UIOn(EUserWidgetType::BossHpbar);
}

// Called every frame
void ATestBossMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MainAnimInst->ChangeAnimation(AniValue);

	if (CurHp != Hp)
	{
		CurHp = Hp;
		BossHP_HUDCheck();
	}
}

void ATestBossMonsterBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	ATestPlayHUD* BossUHD = Cast<ATestPlayHUD>(PlayerController->GetHUD());

	BossUHD->UIOff(EUserWidgetType::BossHpbar);
}

// Called to bind functionality to input
void ATestBossMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestBossMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ATestBossMonsterBase, AniValue);
	DOREPLIFETIME(ATestBossMonsterBase, Hp);
}

void ATestBossMonsterBase::ChangeAniValue(uint8 _Type)
{
	AniValue = _Type;
}

ATestBossMonsterAIControllerBase* ATestBossMonsterBase::GetBossAIController()
{
	return Cast<ATestBossMonsterAIControllerBase>(GetController());
}

void ATestBossMonsterBase::OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MeleeAttack(OtherActor, OtherComp);
}

void ATestBossMonsterBase::MeleeAttack(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	UBlackboardComponent* BlackBoard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	if (nullptr == BlackBoard)
	{
		return;
	}

	EBossMonsterState BossMonsterState = static_cast<EBossMonsterState>(BlackBoard->GetValueAsEnum(TEXT("State")));
	ATestCharacter* HitCharacter = Cast<ATestCharacter>(_OtherActor);
	if (nullptr != HitCharacter && EBossMonsterState::MeleeAttack == BossMonsterState)
	{
		ATestPlayerState* HitPlayerState = Cast<ATestPlayerState>(HitCharacter->GetPlayerState());

		if (nullptr == HitPlayerState)
		{
			LOG(MonsterLog, Fatal, TEXT("HitPlayerState Is Not Valid"));
		}

		HitPlayerState->AddDamage(SettingBossData->Data->GetMeleeAttackDamage());
	}
}

void ATestBossMonsterBase::Damaged(float Damage)
{
	if (false == HasAuthority() || 0.0f >= SettingBossData->HP)
	{
		return;
	}

	//SettingBossData->HP -= Damage;
	Hp -= Damage;

	if (0.0f >= /*SettingBossData->HP*/Hp)
	{
		//SettingBossData->HP = 0;
		Hp = 0.0f;
		OnDead();
	}

	//BossHP_HUDCheck();
}

void ATestBossMonsterBase::SetOnDead_Implementation()
{
	// Effect Setting
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BloodParticle, GetActorTransform());

	// Collision Setting
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel5);
	MeleeAttackComponent->SetCollisionObjectType(ECC_GameTraceChannel5);
	GetCharacterMovement()->SetActive(false);

	SetLifeSpan(5.0f);
}

void ATestBossMonsterBase::BossHP_HUDCheck_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (nullptr == PlayerController)
	{
		LOG(MonsterLog, Fatal, "PlayerController is null");
	}
	ATestPlayHUD* BossUHD = Cast<ATestPlayHUD>(PlayerController->GetHUD());
	if (nullptr == BossUHD)
	{
		LOG(MonsterLog, Fatal, "BossHUD is Bullptr");
		return;
	}

	Cast<UBossHpbarUserWidget>(BossUHD->GetWidget(EUserWidgetType::BossHpbar))->SetHp(/*SettingBossData->HP*/Hp, SettingBossData->Data->GetHP());
}

void ATestBossMonsterBase::OnDead()
{
	SetOnDead();
	ChangeAniValue(EBossMonsterAnim::Dead);

	ATestBossMonsterAIControllerBase* AIController = GetController<ATestBossMonsterAIControllerBase>();
	AIController->GetBrainComponent()->StopLogic("Dead");

	SpawnReportObject();
}

void ATestBossMonsterBase::SpawnReportObject()
{
	UWorld* World = GetWorld();
	if (nullptr == World)
	{
		LOG(MonsterLog, Error, "World is nullptr");
	}

	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z -= 130;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	AReportObject* ReportObject = GetWorld()->SpawnActor<AReportObject>(SpawnLocation, SpawnRotation);
}

void ATestBossMonsterBase::SetAttackCollision(bool Active)
{
	if (true == Active)
	{
		MeleeAttackComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		MeleeAttackComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ATestBossMonsterBase::FireProjectile()
{
	if (ProjectileClass)
	{
		FVector MuzzleLocation = GetActorLocation() + FTransform(GetControlRotation()).TransformVector(MuzzleOffset);

		UWorld* World = GetWorld();
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			ABossProjectile* Projectile = World->SpawnActor<ABossProjectile>(ProjectileClass, MuzzleLocation, FRotator::ZeroRotator, SpawnParams);
			if (Projectile)
			{
				// �߻� ���� ����
				AActor* TargetActor = Cast<AActor>(GetBossAIController()->GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor")));
				FVector LaunchDirection = TargetActor->GetActorLocation() - Projectile->GetActorLocation();

				Projectile->FireInDirection(LaunchDirection);
				Projectile->SetProjectileDamage(SettingBossData->Data->GetRangedAttackDamage());
			}
		}
	}
}