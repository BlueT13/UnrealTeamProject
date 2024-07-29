// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Global/ContentsEnum.h"
#include "GenericTeamAgentInterface.h"
#include "TestPlayerController.generated.h"

/**
 *
 */
UCLASS()
class UNREAL5_PORTFOLIO_API ATestPlayerController : public APlayerController, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	ATestPlayerController();

	// Input
	UPROPERTY()
	class UInputDatas* InputData;
	UFUNCTION(BlueprintCallable)
	void SetupInputComponent() override;

	UFUNCTION()
	void PlayerTick(float DeltaTime) override;

	// Actions
	UFUNCTION(BlueprintCallable)
	void MouseRotation(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void MoveFront(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void MoveBack(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void MoveRight(const FInputActionValue& Value);
	UFUNCTION(BlueprintCallable)
	void MoveLeft(const FInputActionValue& Value);

	//UFUNCTION(BlueprintCallable)
	//void Jump(const FInputActionValue& Value);
	//UFUNCTION(BlueprintCallable)
	//void JumpEnd(const FInputActionValue& Value);

	// ��� �� (��ȯ)
	UFUNCTION(BlueprintCallable)
	void Crouch(const FInputActionValue& Value);

	// Fire
	UFUNCTION(BlueprintCallable)
	void FireStart(float _DeltaTime);
	UFUNCTION(BlueprintCallable)
	void FireTick(float _DeltaTime);
	UFUNCTION(BlueprintCallable)
	void FireEnd();

	// Item
	UFUNCTION(BlueprintCallable)
	void CheckItem();	// => �������� ���� �ʿ� (24.07.29 �߰���) (�Ʒ��� �Լ����� ����)
	//UFUNCTION(BlueprintCallable)
	//void PickUpItem();
	//UFUNCTION(BlueprintCallable)
	//void PickUpItemEnd();

	// State, Posture
	UFUNCTION(BlueprintCallable)
	void ChangeState(EPlayerState _State);
	UFUNCTION(BlueprintCallable)
	void ChangePosture(EPlayerPosture _Posture);

	// POV
	UFUNCTION(BlueprintCallable)
	void ChangePOVController();

	// Reload (k)
	UFUNCTION(BlueprintCallable)
	void IAReload();

	// LowerStateChange �Լ� (��ȯ)
	UFUNCTION(BlueprintCallable)
	void ChangeLowerState(EPlayerLowerState _State);

	// PlayerDirChange �Լ� (��ȯ)
	UFUNCTION(BlueprintCallable)
	void ChangePlayerDir(EPlayerMoveDir _Dir);

	// AttackTest (��ȯ)
	UFUNCTION(BlueprintCallable)
	void AttackMontagePlay();

	//�׽�Ʈ��
	UFUNCTION(BlueprintCallable)
	void SetFaint();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TeamId", meta = (AllowPrivateAccess = true))
	FGenericTeamId TeamId;
	virtual FGenericTeamId GetGenericTeamId() const override;

	FTimerHandle MyTimeHandle;
	int Count = 0;

	UPROPERTY()
	bool PlayerIsFaint = false;

	// HUD / Widget
protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (CallInEditor = true))
	void ChangePostureToWidget(EPlayerPosture _Posture);
};
