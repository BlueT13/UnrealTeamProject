// Fill out your copyright notice in the Description page of Project Settings.


#include "PartDevLevel/Character/TestFPVPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedActionKeyMapping.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "Global/DataAssets/InputDatas.h"
#include "TestFPVCharacter.h"

ATestFPVPlayerController::ATestFPVPlayerController()
{
	TeamId = FGenericTeamId(0);
	{
		FString RefPathString = TEXT("InputDatas'/Game/Resources/Global/DataAssets/DA_InputDatas.DA_InputDatas'");
		ConstructorHelpers::FObjectFinder<UInputDatas> ResPath(*RefPathString);

		if (false == ResPath.Succeeded())
		{
			return;
		}

		InputData = ResPath.Object;
	}
}

void ATestFPVPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FGetItemToWidget_Test.BindUObject(this, &ATestFPVPlayerController::CallGetItem);
}

void ATestFPVPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	{
		UEnhancedInputLocalPlayerSubsystem* InputSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
		UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

		InputSystem->ClearAllMappings();
		InputSystem->AddMappingContext(InputData->InputMapping, 0);

		if (nullptr != InputData->InputMapping)
		{
			EnhancedInputComponent->BindAction(InputData->Actions[0], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::MouseRotation);
			EnhancedInputComponent->BindAction(InputData->Actions[3], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::MoveRight);
			EnhancedInputComponent->BindAction(InputData->Actions[4], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::MoveLeft);
			EnhancedInputComponent->BindAction(InputData->Actions[1], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::MoveFront);
			EnhancedInputComponent->BindAction(InputData->Actions[2], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::MoveBack);
			//EnhancedInputComponent->BindAction(InputData->Actions[5], ETriggerEvent::Started, this, &ATestPlayerController::Jump);
			//EnhancedInputComponent->BindAction(InputData->Actions[5], ETriggerEvent::Completed, this, &ATestPlayerController::JumpEnd);
			//EnhancedInputComponent->BindAction(InputData->Actions[6], ETriggerEvent::Started, this, &ATestPlayerController::FireStart);
			//EnhancedInputComponent->BindAction(InputData->Actions[6], ETriggerEvent::Triggered, this, &ATestPlayerController::FireTick);
			//EnhancedInputComponent->BindAction(InputData->Actions[6], ETriggerEvent::Completed, this, &ATestPlayerController::FireEnd);
			EnhancedInputComponent->BindAction(InputData->Actions[7], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::ChangePosture_Con, static_cast<EPlayerPosture>(0));	// => ���� ���� �ʿ� (24.07.30 �÷��̾� �Լ����� ȥ���� �������� ���� �̸� ������)
			EnhancedInputComponent->BindAction(InputData->Actions[8], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::ChangePosture_Con, static_cast<EPlayerPosture>(1));	// => ���� ���� �ʿ� (24.07.30 �÷��̾� �Լ����� ȥ���� �������� ���� �̸� ������)
			EnhancedInputComponent->BindAction(InputData->Actions[9], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::ChangePosture_Con, static_cast<EPlayerPosture>(2));	// => ���� ���� �ʿ� (24.07.30 �÷��̾� �Լ����� ȥ���� �������� ���� �̸� ������)
			EnhancedInputComponent->BindAction(InputData->Actions[10], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::Drink_Con);			// => ���� ���� �ʿ� (24.07.30 �ش� Ű�� ������ �Լ� �����)
			EnhancedInputComponent->BindAction(InputData->Actions[11], ETriggerEvent::Started, this, &ATestFPVPlayerController::BombSetStart_Con);		// => ���� ���� �ʿ� (24.07.30 �ش� Ű�� ������ �Լ� �����)
			EnhancedInputComponent->BindAction(InputData->Actions[11], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::BombSetTick_Con);	// => ���� ���� �ʿ� (24.07.31 �߰���)
			EnhancedInputComponent->BindAction(InputData->Actions[11], ETriggerEvent::Completed, this, &ATestFPVPlayerController::BombSetCancel_Con);	// => ���� ���� �ʿ� (24.07.30 �ش� Ű�� ������ �Լ� �����)
			EnhancedInputComponent->BindAction(InputData->Actions[20], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::ChangePosture_Con, static_cast<EPlayerPosture>(5));	// => ���� ���� �ʿ� (24.07.30 �ش� �Լ��� ������ Ű �����) ('0' -> 'X')
			EnhancedInputComponent->BindAction(InputData->Actions[13], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::CheckItem_Con);		// => ���� ���� �ʿ� (24.07.29 �ش� Ű�� ������ �Լ� �����) (PickUpItem -> CheckItem)
			EnhancedInputComponent->BindAction(InputData->Actions[14], ETriggerEvent::Triggered, this, &ATestFPVPlayerController::ChangePOV_Con);		// => ���� ���� �ʿ� (24.07.30 �÷��̾� �Լ����� ȥ���� �������� ���� �̸� ������)
			EnhancedInputComponent->BindAction(InputData->Actions[15], ETriggerEvent::Started, this, &ATestFPVPlayerController::Crouch);
			EnhancedInputComponent->BindAction(InputData->Actions[16], ETriggerEvent::Started, this, &ATestFPVPlayerController::IAReload);
		}
	}
}

void ATestFPVPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	if (nullptr == Ch)
	{
		return;
	}

	PlayerIsFaint = Ch->IsFaint;
	PlayerIsBombSetting = Ch->IsBombSetting;
}

void ATestFPVPlayerController::MouseRotation(const FInputActionValue& Value)
{
	FVector2D MouseXY = Value.Get<FVector2D>();
	AddYawInput(MouseXY.X);
	AddPitchInput(-MouseXY.Y);
}

void ATestFPVPlayerController::MoveFront(const FInputActionValue& Value)
{
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	FVector Forward = GetPawn()->GetActorForwardVector();
	GetPawn()->AddMovementInput(Forward);
	ChangePlayerDir(EPlayerMoveDir::Forward);
}

void ATestFPVPlayerController::MoveBack(const FInputActionValue& Value)
{
	// ���� ���� �ʿ�
	// ����, ��ź ��ġ ���� �̵� �Ұ���
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	FVector Forward = GetPawn()->GetActorForwardVector();
	GetPawn()->AddMovementInput(-Forward);
	ChangePlayerDir(EPlayerMoveDir::Back);
}

void ATestFPVPlayerController::MoveRight(const FInputActionValue& Value)
{
	// ���� ���� �ʿ�
	// ����, ��ź ��ġ ���� �̵� �Ұ���
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	FVector Rightward = GetPawn()->GetActorRightVector();
	GetPawn()->AddMovementInput(Rightward);
	ChangePlayerDir(EPlayerMoveDir::Right);
}

void ATestFPVPlayerController::MoveLeft(const FInputActionValue& Value)
{
	// ���� ���� �ʿ�
	// ����, ��ź ��ġ ���� �̵� �Ұ���
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	FVector Rightward = GetPawn()->GetActorRightVector();
	GetPawn()->AddMovementInput(-Rightward);
	ChangePlayerDir(EPlayerMoveDir::Left);
}

void ATestFPVPlayerController::Crouch(const FInputActionValue& Value)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	switch (Ch->LowerStateValue)
	{
	case EPlayerLowerState::Idle:
		Ch->CrouchCameraMove();
		Ch->ChangeLowerState(EPlayerLowerState::Crouch);
		break;
	case EPlayerLowerState::Crouch:
		Ch->CrouchCameraMove();
		Ch->ChangeLowerState(EPlayerLowerState::Idle);
		break;
	default:
		break;
	}
}

//void ATestFPVPlayerController::Jump(const FInputActionValue& Value)
//{
//	ChangeStateController(EPlayerState::Jump);
//	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
//	Ch->Jump();
//}
//
//void ATestFPVPlayerController::JumpEnd()
//{
//	ChangeStateController(EPlayerState::Idle);
//	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
//	Ch->StopJumping();
//}

void ATestFPVPlayerController::FireStart(float _DeltaTime)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();

	if (Ch->CurItemIndex == 3 || Ch->CurItemIndex == 4)
	{
		return;
	}

	Ch->FireRayCast();
	PlayerMontagePlay();
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("Start"));
	GetWorld()->GetTimerManager().SetTimer(MyTimeHandle, FTimerDelegate::CreateLambda([&]()
		{
			FireTick(_DeltaTime);
		}), 0.2f, true);
}

void ATestFPVPlayerController::FireTick(float _DeltaTime)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->FireRayCast();
	++Count;
	PlayerMontagePlay();
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, FString::Printf(TEXT("Tick Count : %d"), Count));
}

void ATestFPVPlayerController::FireEnd()
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	if (Ch->PostureValue == EPlayerPosture::Rifle1 || Ch->PostureValue == EPlayerPosture::Rifle2)
	{
		FireEndMontagePlay();
	}

	GetWorld()->GetTimerManager().ClearTimer(MyTimeHandle);
	GEngine->AddOnScreenDebugMessage(-1, 0.2f, FColor::Red, TEXT("End"));
}

void ATestFPVPlayerController::Drink_Con()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��) => ���� ����(�ּ�)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->Drink();
}

void ATestFPVPlayerController::BombSetStart_Con()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��) => ���� ����(�ּ�)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->BombSetStart();
}

void ATestFPVPlayerController::BombSetTick_Con()	// => ���ο� ���� �����ؾ� �� (24.07.31 �߰� �� �׽��� ��)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->BombSetTick();
}

void ATestFPVPlayerController::BombSetCancel_Con()	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��) => ���� ����(�ּ�)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->BombSetCancel();
}

void ATestFPVPlayerController::CheckItem_Con()	// => �������� ���� �ʿ� (24.07.29 �߰���) => ���� ����
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->CheckItem();
}

void ATestFPVPlayerController::ChangePosture_Con(EPlayerPosture _Posture)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangePosture(_Posture);
	ChangePostureToWidget(_Posture);
}

void ATestFPVPlayerController::ChangePOV_Con()
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangePOV();
}

void ATestFPVPlayerController::IAReload()
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->CharacterReload();
}

void ATestFPVPlayerController::ChangeLowerState(EPlayerLowerState _State)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangeLowerState(_State);
}

void ATestFPVPlayerController::ChangePlayerDir(EPlayerMoveDir _Dir)
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangePlayerDir(_Dir);
}

void ATestFPVPlayerController::PlayerMontagePlay()
{
	// ���� ���� �ʿ�
	// ����, ��ź ��ġ ���� ��Ÿ�� ���� �Ұ���
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangeMontage(false);
}

void ATestFPVPlayerController::FireEndMontagePlay()
{
	// ���� ���� �ʿ�
	// ����, ��ź ��ġ ���� ��Ÿ�� ���� �Ұ���
	if (PlayerIsFaint == true || PlayerIsBombSetting == true)
	{
		return;
	}

	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangeMontage(true);
}

void ATestFPVPlayerController::SetFaint()
{
	ATestFPVCharacter* Ch = GetPawn<ATestFPVCharacter>();
	Ch->ChangeIsFaint();
}

FGenericTeamId ATestFPVPlayerController::GetGenericTeamId() const
{
	return TeamId;
}

void ATestFPVPlayerController::CallGetItem()
{
	CallGetItemToWidget();
}
