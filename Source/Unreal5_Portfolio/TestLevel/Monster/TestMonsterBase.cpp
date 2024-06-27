// Fill out your copyright notice in the Description page of Project Settings.


#include "TestLevel/Monster/TestMonsterBase.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/ContentsEnum.h"

// Sets default values
ATestMonsterBase::ATestMonsterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	UMainGameInstance* MainGameInst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());

	// Ŭ���̾�Ʈ�� ���� �Ұ� ����.
	if (nullptr == GetController())
	{
		return;
	}

	BaseData = MainGameInst->GetMonsterData(BaseDataName);
	if (nullptr == BaseData)
	{
		UE_LOG(LogTemp, Fatal, TEXT("%S(%u)> if (PortNumber == 0)"), __FUNCTION__, __LINE__);
		return;
	}

	//  ���� AI ������ ����
	SettingData = NewObject<UMonsterData>(this);
	SettingData->Data = BaseData;
}

// Called every frame
void ATestMonsterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestMonsterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

