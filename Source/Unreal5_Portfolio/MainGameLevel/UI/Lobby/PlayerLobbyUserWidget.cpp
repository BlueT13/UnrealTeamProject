// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/UI/Lobby/PlayerLobbyUserWidget.h"
#include "MainGameLevel/UI/Lobby/LobbyCharacter.h"
#include "MainGameLevel/UI/Lobby/PlayerViewUserWidget.h"
#include "MainGameLevel/UI/Lobby/CharacterSelectionUserWidget.h"
#include "MainGameLevel/UI/Title/ServerBtnUserWidget.h"
#include "MainGameLevel/LobbyGameMode.h"

#include "Kismet/GameplayStatics.h"

void UPlayerLobbyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// ��ư Ŭ�� �� ���� �Լ� ������
	B_Ready->DefaultButton->OnClicked.AddUniqueDynamic(this, &UPlayerLobbyUserWidget::OnReadyBtn);
	B_Start->DefaultButton->OnClicked.AddUniqueDynamic(this, &UPlayerLobbyUserWidget::OnStartBtn);

	// ��ư �ؽ�Ʈ ����
	B_Ready->SetButtonText(FText::FromString(FString("Ready")));
	B_Start->SetButtonText(FText::FromString(FString("Start")));

	AGameModeBase* IsServer = GetWorld()->GetAuthGameMode();
	if (nullptr == IsServer)
	{
		// Client
		ReadybtnVisibility(true);
		StartbtnVisibility(false);
	}
	else
	{
		// Server
		ReadybtnVisibility(false);
		StartbtnVisibility(true);
	}
}

void UPlayerLobbyUserWidget::ReadybtnVisibility(bool _On)
{
	if (true == _On)
	{
		B_Ready->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		B_Ready->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerLobbyUserWidget::StartbtnVisibility(bool _On)
{
	if (true == _On)
	{
		B_Start->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		B_Start->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UPlayerLobbyUserWidget::OnReadyBtn()
{
	ALobbyCharacter* MyLobbyCharacter = Cast<ALobbyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	MyLobbyCharacter->ReadyClicked = true;
}

void UPlayerLobbyUserWidget::OnStartBtn()
{
	AGameModeBase* IsServer = GetWorld()->GetAuthGameMode();
	ALobbyGameMode* lobby = Cast<ALobbyGameMode>(IsServer);

	int RCount = lobby->GetReadyCount();
	int PCount = lobby->GetPlayerCount();

	if (RCount == PCount - 1)
	{
		// ��� �غ� ��
		// �������� �̵�
	}
	else
	{
		// ������ �� ����.
		int a = 0;
	}
}

void UPlayerLobbyUserWidget::LobbyPlayerName(int _Order, FText _Name)
{
	switch (_Order)
	{
	case 0:
		CurPlayerView->SetP0Name(_Name);
		break;
	case 1:
		CurPlayerView->SetP1Name(_Name);
		break;
	case 2:
		CurPlayerView->SetP2Name(_Name);
		break;
	case 3:
		CurPlayerView->SetP3Name(_Name);
		break;
	default:
		CurPlayerView->SetP0Name(_Name);
		break;
	}
}

void UPlayerLobbyUserWidget::LobbyPlayerView(int _Order, int _CharacterType)
{
	CurPlayerView->SetPlayerView(_Order, _CharacterType);
}
