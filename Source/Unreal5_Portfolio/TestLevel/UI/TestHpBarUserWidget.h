// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TestHpBarUserWidget.generated.h"

/**
 * ��ü HpBar ������ ��� �ִ� �ְ� �� Ŭ������ ���� ��.
 */
UCLASS()
class UNREAL5_PORTFOLIO_API UTestHpBarUserWidget : public UUserWidget
{
	GENERATED_BODY()

	// ���� �������
	UPROPERTY(meta = (BindWidget))
	class UDefaultHpBarUserWidget* MainPlayer;

	UPROPERTY(meta = (BindWidget))
	class UDefaultHpBarUserWidget* Player2;

	UPROPERTY(meta = (BindWidget))
	class UDefaultHpBarUserWidget* Player3;

	UPROPERTY(meta = (BindWidget))
	class UDefaultHpBarUserWidget* Player4;

public:
	void NativeConstruct() override;
	//void NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime) override;

	void HpbarUpdate(int _Token, float _CurHp, float _MaxHp);
	void NickNameUpdate(int _Token, FText _nickname);

private:
	void WidgetInit();
	TArray<UDefaultHpBarUserWidget*> HpWidgets;
};
