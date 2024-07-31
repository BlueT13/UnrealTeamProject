// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/UI/Lobby/LobbyCharacter.h"
#include "MainGameLevel/LobbyGameMode.h"

// Sets default values
ALobbyCharacter::ALobbyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALobbyCharacter::BeginPlay()
{
	Super::BeginPlay();

	IsServerPtr = GetWorld()->GetAuthGameMode();
	if (nullptr != IsServerPtr)
	{
		// Server�� ���� ĳ���Ͷ�� ���Ӹ���� PlayerCount�� + 1
		ALobbyGameMode* lobby = Cast<ALobbyGameMode>(IsServerPtr);
		lobby->SetPlayerCount(lobby->GetPlayerCount() + 1);
	}
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
}

// Called to bind functionality to input
void ALobbyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}


void ALobbyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}


void ALobbyCharacter::ClientReady_Implementation()
{
	ALobbyGameMode* lobby = Cast<ALobbyGameMode>(IsServerPtr);
	lobby->SetReadyCount(lobby->GetReadyCount() + 1);
}
