// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/UI/Lobby/LobbyCharacter.h"
#include "MainGameLevel/LobbyGameMode.h"
#include "MainGameLevel/UI/Lobby/MainLobbyHUD.h"
#include "MainGameLevel/UI/Lobby/PlayerLobbyUserWidget.h"
#include "MainGameLevel/UI/Lobby/LobbyCapCharacter.h"

#include "Global/MainGameInstance.h"
#include "Global/ContentsLog.h"
#include "Global/MainGameBlueprintFunctionLibrary.h"

#include "Blueprint/UserWidget.h"

#include "Kismet/GameplayStatics.h"

// Sets default values
ALobbyCharacter::ALobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALobbyCharacter::BeginPlay()
{
	IsServerPtr = GetWorld()->GetAuthGameMode();
	Super::BeginPlay();

	if (nullptr != IsServerPtr)
	{
		// Server�� ���� ĳ���Ͷ�� ���Ӹ���� PlayerCount�� + 1
		ALobbyGameMode* lobby = Cast<ALobbyGameMode>(IsServerPtr);
		int pc = lobby->GetPlayerCount();
		lobby->SetPlayerCount(pc + 1);

		MyOrder = pc; // 0, 1, 2, 3
	}
	// �� �÷��̾ �ش��ϴ� ����ŷ�� BP���� Spawn�� 
	MyMannequin->SetMyNumber(MyOrder);
}

// Called every frame
void ALobbyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// �غ� �Ϸ� �� �������� �˷���. 
	if (true == ReadyClicked)
	{
		ClientReady();
		ReadyClicked = false;
	}

	if (false == SetWidgetNickName) 
	{
		SetWidgetNickName = true;

		UMainGameInstance* Inst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
		if (nullptr == Inst)
		{
			LOG(UILog, Fatal, "MainGameInstance is Null");
		}

		APlayerController* Con = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (nullptr != Con)
		{
			AMainLobbyHUD* MyHUD = Cast<AMainLobbyHUD>(Con->GetHUD());		
			Cast<UPlayerLobbyUserWidget>(MyHUD->GetWidget(EUserWidgetType::LobbyButton))->LobbyPlayerName(MyOrder, FText::FromString(Inst->GetMainNickName()));
		}
	}

	UMainGameInstance* Inst = UMainGameBlueprintFunctionLibrary::GetMainGameInstance(GetWorld());
	if (nullptr == Inst)
	{
		LOG(UILog, Fatal, "MainGameInstance is Null");
	}

	// ����ŷ�� �÷��̾�� ���� ������Ʈ
	MyMannequin->SetEachMesh(MyChracterType);

	int a = 0;
}

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyCharacter, MyOrder);
	DOREPLIFETIME(ALobbyCharacter, MyChracterType);
}


void ALobbyCharacter::ClientReady_Implementation()
{
	ALobbyGameMode* lobby = Cast<ALobbyGameMode>(IsServerPtr);
	if (nullptr == lobby)
	{
		return;
	}

	lobby->SetReadyCount(lobby->GetReadyCount() + 1);
}

void ALobbyCharacter::ClientChangedMesh_Implementation(FName _NewType)
{
	MyChracterType = _NewType;
}
