// Fill out your copyright notice in the Description page of Project Settings.


#include "TestLevel/UI/TestPlayHUD.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"

void ATestPlayHUD::BeginPlay()
{
	Super::BeginPlay();

	//UMainGameInstance* Inst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());

	//TMap<FString, TSubclassOf<UUserWidget>>& AllUI = Inst->GetTPSPlayWidgets();

	//// ���ʿ� �����Ҷ��� 
	//for (TPair<FString, TSubclassOf<UUserWidget>> Pair : AllUI)
	//{
	//	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), Pair.Value);
	//	Widget->AddToViewport();
	//	Widgets.Add(Pair.Key, Widget);
	//}


}