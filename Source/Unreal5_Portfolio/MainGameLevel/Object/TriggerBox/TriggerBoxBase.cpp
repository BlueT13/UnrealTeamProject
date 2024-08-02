// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameLevel/Object/TriggerBox/TriggerBoxBase.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "LevelSequence.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"


#include "Global/ContentsLog.h"

ATriggerBoxBase::ATriggerBoxBase()
{
	// Ʈ���� �ڽ� ������Ʈ �ʱ�ȭ
	OnActorBeginOverlap.AddDynamic(this, &ATriggerBoxBase::OnOverlapBegin);

	// ��Ʈ��ũ ����
	//bReplicates = true;
	//bAlwaysRelevant = true;
}

void ATriggerBoxBase::BeginPlay()
{
	Super::BeginPlay();

	CenterLocation = GetActorLocation();
}

void ATriggerBoxBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATriggerBoxBase, CenterLocation);
	DOREPLIFETIME(ATriggerBoxBase, LevelSequenceAsset);
	DOREPLIFETIME(ATriggerBoxBase, IsPlayerTP);
}

void ATriggerBoxBase::OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor)
{
	if (false == HasAuthority())
	{
		LOG(ObjectLog, Error, "������ �ƴմϴ�.");
		return;
	}

	if (true == IsPlayerTP)
	{
		return;
	}

	IsPlayerTP = true;
	SetAllPlayersLocation(CenterLocation);

	for (FConstPlayerControllerIterator PlayerIt = GetWorld()->GetPlayerControllerIterator(); PlayerIt; ++PlayerIt)
	{
		if (APlayerController* PlayerController = PlayerIt->Get())
		{
			DisablePlayerInput(PlayerController);
		}
	}

	// �ó׸�ƽ ������ ���
	PlayCinematicSequence();

}

void ATriggerBoxBase::SetAllPlayersLocation_Implementation(const FVector& NewLocation)
{

	const float DistanceFromCenter = 300.0f;

	TArray<FVector> PlayerLocations;
	PlayerLocations.Add(FVector(CenterLocation.X, CenterLocation.Y - (DistanceFromCenter * 2), CenterLocation.Z));
	PlayerLocations.Add(FVector(CenterLocation.X, CenterLocation.Y - DistanceFromCenter, CenterLocation.Z));
	PlayerLocations.Add(FVector(CenterLocation.X, CenterLocation.Y + DistanceFromCenter, CenterLocation.Z));
	PlayerLocations.Add(FVector(CenterLocation.X, CenterLocation.Y + (DistanceFromCenter * 2), CenterLocation.Z));

	// �÷��̾� ��Ʈ�ѷ� ��ȸ �� ��ġ ����
	int32 PlayerIndex = 0;
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		if (APlayerController* PlayerController = It->Get())
		{
			if (APawn* PlayerPawn = PlayerController->GetPawn())
			{
				// �÷��̾ �ڽ� �ֺ��� ��ġ
				if (PlayerIndex < PlayerLocations.Num())
				{
					PlayerPawn->SetActorLocation(PlayerLocations[PlayerIndex]);

					// ī�޶� �ٶ� ���� ���� (Z���� 0���� ����)
					FVector CameraDirection = FVector(0.0f, 0.0f, 0.0f); // ���ϴ� ���� ����
					FRotator LookAtRotation = FRotationMatrix::MakeFromX(CameraDirection).Rotator();

					if (ACharacter* Character = Cast<ACharacter>(PlayerPawn))
					{
						UCameraComponent* CameraComponent = Character->FindComponentByClass<UCameraComponent>();
						if (CameraComponent)
						{
							//CameraComponent->SetWorldRotation(LookAtRotation);
							//PlayerPawn->SetActorRotation(FRotator(0,90,0));
						}
					}

					PlayerIndex++;
				}
			}
		}
	}

	//// ��� �÷��̾��� ��ġ�� Ư�� �������� ����
	//for (FConstPlayerControllerIterator PlayerIt = GetWorld()->GetPlayerControllerIterator(); PlayerIt; ++PlayerIt)
	//{
	//	if (APlayerController* PlayerController = PlayerIt->Get())
	//	{
	//		if (APawn* PlayerPawn = PlayerController->GetPawn())
	//		{
	//			PlayerPawn->SetActorLocation(NewLocation);
	//		}
	//	}
	//}
}

void ATriggerBoxBase::PlayCinematicSequence_Implementation()
{
	if (!LevelSequenceAsset)
	{
		LOG(ObjectLog, Error, "�ó׸�ƽ ������ �Ǿ����� �ʽ��ϴ�.");
		return;
	}

	// �ó׸�ƽ �������� ���
	FMovieSceneSequencePlaybackSettings Settings;
	ALevelSequenceActor* OutActor;

	// LevelSequencePlayer ����
	ULevelSequencePlayer* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(
		GetWorld(), LevelSequenceAsset, Settings, OutActor);

	if (SequencePlayer)
	{
		SequencePlayer->Play();

		// �ó׸�ƽ �������� ������ �� ȣ��Ǵ� ��������Ʈ ���ε�
		SequencePlayer->OnFinished.AddDynamic(this, &ATriggerBoxBase::OnSequenceFinished);
	}
}

void ATriggerBoxBase::OnSequenceFinished()
{
	// ��� �÷��̾��� �Է��� �ٽ� Ȱ��ȭ
	for (FConstPlayerControllerIterator PlayerIt = GetWorld()->GetPlayerControllerIterator(); PlayerIt; ++PlayerIt)
	{
		if (APlayerController* PlayerController = PlayerIt->Get())
		{
			EnablePlayerInput(PlayerController);
		}
	}

	Destroy();
}

void ATriggerBoxBase::DisablePlayerInput_Implementation(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->GetPawn())
	{
		PlayerController->GetPawn()->DisableInput(PlayerController);
		PlayerController->DisableInput(PlayerController);
	}
}

void ATriggerBoxBase::EnablePlayerInput_Implementation(APlayerController* PlayerController)
{
	if (PlayerController && PlayerController->GetPawn())
	{
		PlayerController->GetPawn()->EnableInput(PlayerController);
		PlayerController->EnableInput(PlayerController);
	}
}
