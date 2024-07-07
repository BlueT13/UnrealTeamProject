// Fill out your copyright notice in the Description page of Project Settings.


#include "TestLevel/Monster/TestMonsterBase.h"
#include "TestMonsterBaseAIController.h"
#include "TestLevel/Character/TestCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFrameWork/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/Animation/MainAnimInstance.h"
#include "Global/ContentsEnum.h"
#include "Global/ContentsLog.h"


// Sets default values
ATestMonsterBase::ATestMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	LeftAttackComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Left Attack Comp"));
	LeftAttackComponent->SetupAttachment(GetMesh(), FName(TEXT("LeftAttackPos")));
	
	RightAttackComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Right Attack Comp"));
	RightAttackComponent->SetupAttachment(GetMesh(), FName(TEXT("RightAttackPos")));

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);

}

// Called when the game starts or when spawned
void ATestMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());

	UAnimInstance* Inst = GetMesh()->GetAnimInstance();
	AnimInst = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());
	BaseData = MainGameInst->GetMonsterData(BaseDataName);

	if (nullptr == BaseData)
	{
		LOG(MonsterLog, Fatal, TEXT("BaseData Is Null"));
		return;
	}

	TMap<EMonsterAnim, UAnimMontage*> AnimMontages = BaseData->GetAnimMontage();
	for (TPair<EMonsterAnim, class UAnimMontage*> Montage : AnimMontages)
	{
		AnimInst->PushAnimation(Montage.Key, Montage.Value);
	}

	//  ���� ������ ����
	SettingData = NewObject<UMonsterData>(this);
	SettingData->Data = BaseData;
	SettingData->OriginPos = GetActorLocation();

	// Ŭ���̾�Ʈ�� ���
	ATestMonsterBaseAIController* AIController = GetController<ATestMonsterBaseAIController>();
	if (nullptr == AIController)
	{
		return;
	}

	AIController->GetBlackboardComponent()->SetValueAsObject(TEXT("MonsterData"), SettingData);
}

// Called every frame
void ATestMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AnimInst->ChangeAnimation(AniValue);
}

// Called to bind functionality to input
void ATestMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ATestMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATestMonsterBase, AniValue);
}

ATestMonsterBaseAIController* ATestMonsterBase::GetAIController()
{
	return Cast<ATestMonsterBaseAIController>(GetController());
}

UMainAnimInstance* ATestMonsterBase::GetAnimInstance()
{
	return AnimInst;
}

void ATestMonsterBase::ChangeAnimation_Implementation(uint8 _Type)
{
	AniValue = _Type;
}

void ATestMonsterBase::SetDeadCollision_Implementation()
{
	GetCapsuleComponent()->SetCollisionObjectType(ECC_GameTraceChannel5);
	RightAttackComponent->SetCollisionObjectType(ECC_GameTraceChannel5);
	LeftAttackComponent->SetCollisionObjectType(ECC_GameTraceChannel5);
}

void ATestMonsterBase::Attack(AActor* _OtherActor, UPrimitiveComponent* _Collision)
{
	UBlackboardComponent* BlackBoard = UAIBlueprintHelperLibrary::GetBlackboard(this);
	if (nullptr == BlackBoard)
	{
		return;
	}
	
	EMonsterState MonsterState = static_cast<EMonsterState>(BlackBoard->GetValueAsEnum(TEXT("State")));
	ATestCharacter* HitCharacter = Cast<ATestCharacter>(_OtherActor);
	if (nullptr != HitCharacter && EMonsterState::Attack == MonsterState)
	{
		IsCharacterHit = true;
	}
}

void ATestMonsterBase::GetDamage(float Damage)
{
	SettingData->Hp -= Damage;

	if (0.0f >= SettingData->Hp)
	{
		ATestMonsterBaseAIController* AIController = GetController<ATestMonsterBaseAIController>();
		if (nullptr != AIController)
		{
			AIController->UnPossess();
		}

		SetDeadCollision();
		GetCharacterMovement()->SetActive(false);
		ChangeAnimation(EMonsterAnim::Dead);
	}
}
