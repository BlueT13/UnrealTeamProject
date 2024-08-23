// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/MainGameMode.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"
#include "Global/MainGameInstance.h"
#include "Character/TestPlayerState.h"

#include "Global/ContentsEnum.h"

void AMainGameMode::BeginPlay()
{
	Super::BeginPlay();

	//UMainGameBlueprintFunctionLibrary::GetMainGameState(GetWorld())->SetCurStage(EGameStage::MoveToGatheringPoint);
}

void AMainGameMode::Tick(float _DeltaTime)
{
	Super::Tick(_DeltaTime);
}