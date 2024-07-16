// Fill out your copyright notice in the Description page of Project Settings.


#include "TestLevel/UI/TestTeamHpBarUserWidget.h"
#include "Components/ProgressBar.h"
#include "TestPlayHUD.h"
#include "Kismet/GameplayStatics.h"
#include "TestLevel/Character/TestPlayerState.h"
#include "TestLevel/TestPlayMode.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/DataTable/PlayerDataRow.h"

void UTestTeamHpBarUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// �� PlayerNumber �޾Ƴ���
	ATestPlayerState* MyPlayerState = Cast<ATestPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	if (MyPlayerState != nullptr)
	{
		MyPlayerNumber = MyPlayerState->GetPlayerNumber();
	}
	
	// �ִ� ü�°� �޾Ƶα�
	UMainGameInstance* Inst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
	MaxHp = static_cast<float>(Inst->GetPlayerData(FName("TestPlayer"))->GetHp());

	// �� ���α׷����� Array�� �־�α�
	int PNum = 2;
	for (int i = 0; i < 4; ++i)
	{
		if(i == MyPlayerNumber)
		{
			HpArray.Add(nullptr);
			continue;
		}
		switch (PNum)
		{
		case 2:
			HpArray.Add(PB_P2HpBar);
			++PNum;
			break;
		case 3:
			HpArray.Add(PB_P3HpBar);
			++PNum;
			break;
		case 4:
			HpArray.Add(PB_P4HpBar);
			++PNum;
			break;
		default:
			break;
		}
		
	}
}

void UTestTeamHpBarUserWidget::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
{
	Super::NativeTick(_MyGeometry, _InDeltaTime);

	HPUpdate();
}

void UTestTeamHpBarUserWidget::HPUpdate()
{
	ATestPlayerState* MyPlayerState = Cast<ATestPlayerState>(UGameplayStatics::GetPlayerState(GetWorld(), 0));
	if (MyPlayerState == nullptr)
	{
		return;
	}
	if (PlayerCount)
	{

	}

	TArray<ATestPlayerState*> AllPlayerStates = MyPlayerState->GetAllPlayerStates();

	for (int i = 0; i < 4; ++i)
	{
		if (MyPlayerNumber == i)
		{
			continue;
		}
		if (i >= AllPlayerStates.Num())
		{
			break;
		}
		// ���� �ƴ� �ֵ��� Hp ��������?
		HpArray[i]->SetPercent(AllPlayerStates[i]->GetPlayerHp() / MaxHp);
	}
}