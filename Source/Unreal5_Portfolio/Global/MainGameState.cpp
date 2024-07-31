// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/MainGameState.h"
#include "Net/UnrealNetwork.h"
#include "Global/ContentsEnum.h"
#include "Global/ContentsLog.h"

// ���� �ٲ���� ��� MainCharacter�� �����ؾ� ��
#include "TestLevel/Character/TestCharacter.h"



AMainGameState::AMainGameState()
{

}

void AMainGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMainGameState, CurStage);
	DOREPLIFETIME(AMainGameState, MeleeNum);
	DOREPLIFETIME(AMainGameState, RifleNum);
	DOREPLIFETIME(AMainGameState, BombNum);
	DOREPLIFETIME(AMainGameState, ClearBoss1Stage);
	DOREPLIFETIME(AMainGameState, ClearBoss2Stage);
}

void AMainGameState::PushActor(uint8 _Index, AActor* _Actor)
{
	if (false == AllActor.Contains(_Index))
	{
		AllActor.Add(_Index, NewObject<UActorGroup>());
	}

	AllActor[_Index]->Actors.Add(_Actor);
}

UActorGroup* AMainGameState::GetActorGroup(uint8 _Index)
{
	if (false == AllActor.Contains(_Index))
	{
		AllActor.Add(_Index, NewObject<UActorGroup>());
	}

	return AllActor[_Index];
}

void AMainGameState::GameStateCheck_Implementation()
{
	if (false == HasAuthority())
	{
		return;
	}

	if (MaxPlayerCount == PlayerCount)
	{
		UActorGroup* PlayerGroup = GetActorGroup<EObjectType>(EObjectType::Player);
		
		if (nullptr == PlayerGroup)
		{
			LOG(GlobalLog, Fatal, "if (nullptr == PlayerGroup)");
			return;
		}

		switch (CurStage)
		{
		case EGameStage::Init:
		{
			for (int i = 0; i < PlayerGroup->Actors.Num(); i++)
			{
				// ���� ���� Player�� ���� �ʿ�
				ATestCharacter* Player = Cast<ATestCharacter>(PlayerGroup->Actors[i]);

				if (true == Player->IsItemIn[static_cast<int>(EPlayerPosture::Melee)])
				{
					++ItemCount;
				}
			}

			if (MaxItemCount == ItemCount)
			{
				CurStage = EGameStage::VisitArmory;
				PlayerCount = 0;
			}

			ItemCount = 0;
			break;
		}
		case EGameStage::VisitArmory:
		{
			break;
		}
		case EGameStage::ObtainFirstSample:
			break;
		case EGameStage::ObtainSecondSample:
			break;
		case EGameStage::ObtainThirdSample:
			break;
		case EGameStage::PlantingBomb:
			break;
		case EGameStage::MoveToGatheringPoint:
			break;
		case EGameStage::Defensing:
			break;
		case EGameStage::MissionClear:
			break;
		default:
			break;
		}
	}
}
