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
		HpWidgets[i]->SetNickName(FText::FromString(NameInit));
	}

	// HP �ʱ�ȭ
	for (int i = 0; i < 4; ++i)
	{
		HpWidgets[i]->SetHp(1.f);
	}
}

//void UTestHpBarUserWidget::NativeTick(const FGeometry& _MyGeometry, float _InDeltaTime)
//{
//	Super::NativeTick(_MyGeometry, _InDeltaTime);
//
//}

// �� �Լ��� ���� �÷��̾ �����ؾ� �մϴ�. (�÷����ϴ� ��ǻ�ʹ� 1ȸ ����)
void UTestHpBarUserWidget::HpbarInit_ForMainPlayer(int _MainPlayerToken)
{
	MainPlayerIndex = _MainPlayerToken;

	for (int i = 0; i < 4; ++i)
	{
		if (i == MainPlayerIndex)
		{
			continue;
		}
		OtherPlayerNum.Add(i);
	}
}

void UTestHpBarUserWidget::HpbarUpdate(int _Token, float _CurHp, float _MaxHp)
{
	// �׽�Ʈ��
	//if (_Token == 1)
	//{
	//	int a = 0;
	//}
	//else if (_Token == 0)
	//{
	//	int a = 0;
	//	_CurHp = 40.f;
	//}

	//if (MainPlayerIndex == 1)
	//{
	//	int a = 0;
	//}
	//else if (MainPlayerIndex == 0)
	//{
	//	int a = 0;
	//}

	if (MainPlayerIndex == _Token)
	{
		HpWidgets[0]->SetHp(_CurHp / _MaxHp);
		return;
	}

	for (int i = 0; i < 3; ++i)
	{
		if (OtherPlayerNum[i] == _Token)
		{
			HpWidgets[i + 1]->SetHp(_CurHp / _MaxHp);
			return;
		}
	}
	
}

void UTestHpBarUserWidget::NickNameUpdate(int _Token, FText _nickname)
{
	if (MainPlayerIndex == _Token)
	{
		HpWidgets[0]->SetNickName(_nickname);
		return;
	}

	for (int i = 0; i < 3; ++i)
	{
		if (OtherPlayerNum[i] == _Token)
		{
			HpWidgets[i + 1]->SetNickName(_nickname);
			return;
		}
	}
}


