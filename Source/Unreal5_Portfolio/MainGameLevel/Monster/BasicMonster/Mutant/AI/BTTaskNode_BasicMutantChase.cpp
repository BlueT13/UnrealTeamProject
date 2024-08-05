// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_BasicMutantChase.h"
#include "MainGameLevel/Monster/BasicMonster/Mutant/BasicMutant.h"
#include "MainGameLevel/Monster/BasicMonster/Mutant/BasicMutantData.h"
#include "MainGameLevel/Monster/BasicMonster/AI/BasicMonsterAIController.h"

#include "GameFramework/CharacterMovementComponent.h"

#include "Global/ContentsLog.h"

EBTNodeResult::Type UBTTaskNode_BasicMutantChase::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ABasicMutant* Mutant = GetSelfActor<ABasicMutant>(OwnerComp);
	if (false == Mutant->IsValidLowLevel())
	{
		LOG(MonsterLog, Fatal, TEXT("Mutant Is Not Valid"));
		return EBTNodeResult::Type::Aborted;
	}

	UBasicMutantData* MutantData = Mutant->GetSettingData();
	if (false == Mutant->IsValidLowLevel())
	{
		LOG(MonsterLog, Fatal, TEXT("MutantData Is Not Valid"));
		return EBTNodeResult::Aborted;
	}

	Mutant->GetCharacterMovement()->MaxWalkSpeed = MutantData->BaseData->GetRunSpeed();
	Mutant->ChangeRandomAnimation(EBasicMonsterAnim::Run);

	return EBTNodeResult::InProgress;
}

void UBTTaskNode_BasicMutantChase::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* pNodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, pNodeMemory, DeltaSeconds);

	if (EBasicMonsterState::Chase != static_cast<EBasicMonsterState>(GetCurState(OwnerComp)))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABasicMutant* Mutant = GetSelfActor<ABasicMutant>(OwnerComp);
	FVector MutantLocation = Mutant->GetActorLocation();

	AActor* Target = GetValueAsObject<AActor>(OwnerComp, TEXT("TargetActor"));
	FVector TargetLocation = Target->GetActorLocation();

	// Attack Range Check
	UBasicMutantData* MutantData = Mutant->GetSettingData();
	FVector LocationDiff = TargetLocation - MutantLocation;
	float DiffLength = LocationDiff.Size();
	if (DiffLength <= MutantData->AttackRange)
	{
		StateChange(OwnerComp, EBasicMonsterState::Attack);
		return;
	}
	else if (DiffLength <= MutantData->JumpAttackMaxRange && DiffLength >= MutantData->JumpAttackMinRange)
	{
		StateChange(OwnerComp, EBasicMonsterState::JumpAttack);
		return;
	}

	// Move
	Mutant->GetAIController()->MoveToLocation(TargetLocation);
}
