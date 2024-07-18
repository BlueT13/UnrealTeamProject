// Fill out your copyright notice in the Description page of Project Settings.


#include "TestHpBarUserWidget.h"
#include "DefaultHpBarUserWidget.h"


void UTestHpBarUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	WidgetInit();
}

void UTestHpBarUserWidget::WidgetInit()
{
	// HpWidgets ����
	HpWidgets.Add(MainPlayer);
	HpWidgets.Add(Player2);
	HpWidgets.Add(Player3);
	HpWidgets.Add(Player4);

	// NickName �ʱ�ȭ
	for (int i = 0; i < 4; ++i)
	{
		FString NameInit = "Player" + FString::FromInt(i);
		NickNameUpdate(i, FText::FromString(NameInit));
	}

	// HP �ʱ�ȭ
	for (int i = 0; i < 4; ++i)
	{
		HpbarUpdate(i, 10.f, 10.f);
	}
}

//void UTestHpBarUserWidget::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
//{
//	Super::NativeTick(_MyGeometry, _InDeltaTime);
//
//}

void UTestHpBarUserWidget::HpbarUpdate(int _Token, float _CurHp, float _MaxHp)
{
	HpWidgets[_Token]->SetHp(_CurHp / _MaxHp);
}

void UTestHpBarUserWidget::NickNameUpdate(int _Token, FText _nickname)
{
	HpWidgets[_Token]->SetNickName(_nickname);
}


