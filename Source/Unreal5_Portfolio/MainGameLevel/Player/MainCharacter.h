// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Global/ContentsEnum.h"
#include "Net/UnrealNetwork.h"
#include "MainCharacter.generated.h"

UCLASS()
class UNREAL5_PORTFOLIO_API AMainCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private : // ���� �߻� ���� ���� �߻��ϸ� �׳� ������ ��.
	// == Components ==
	
	// ��������
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent = nullptr;
	// ī�޶�
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent = nullptr;
	// ����Ī �޽�
	UPROPERTY(Category = "Contents", VisibleDefaultsOnly)
	USkeletalMeshComponent* FPVMesh = nullptr;
	// �̴ϸ� ������
	//UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UTestMinimapIconComponent* MinimapIconComponent = nullptr;
	// �ʿ� �ִ� ������ Ž�� �ݸ���
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GetMapItemCollisonComponent = nullptr;
	// ������ ������ ���� ��ġ
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CreateItemComponent = nullptr;

	// ��Ÿ�� ������ ���� AnimInstance
	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInst;


	// == State, Posture ==
	
	// �÷��̾� �ڼ� ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerPosture PostureValue = EPlayerPosture::Barehand;


	// == ��Ī ���� ���� ==
	UPROPERTY()
	bool IsFPV = true;

	// == Inventory ==




	// �ʿ� �ִ� ���� Data
	UPROPERTY(Category = "Contents", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* GetMapItemData = nullptr;

	// ���� ������ Index
	UPROPERTY(VisibleAnywhere)
	int CurItemIndex = -1;

	// ���� ������ ����.
	UPROPERTY(VisibleAnywhere)
	TArray<struct FPlayerItemInformation> ItemSlot;

	// ��ü ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerLowerState LowerStateValue = EPlayerLowerState::Idle;

	// ��ü ����


	// == Server ==
public :
	// ��ä ����
	UFUNCTION(Reliable, Server)
	void ChangePosture(EPlayerPosture _Type);
	void ChangePosture_Implementation(EPlayerPosture _Type);

	// ���� ����
	UFUNCTION(Reliable, Server)
	void ChangeLowerState();
	void ChangeLowerState_Implementation();

	// ������ ����
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void PickUpItem();
	void PickUpItem_Implementation();

	// ������ ���� -> ���
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void CharacterPlayerToDropItem(FName _ItemName, FTransform _Transform);
	void CharacterPlayerToDropItem_Implementation(FName _ItemName, FTransform _Transform);

	// Fire Ray Cast
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void FireRayCast(float _DeltaTime);
	void FireRayCast_Implementation(float _DeltaTime);

	// ���� ��Ÿ�� ����
	UFUNCTION(Reliable, Server)
	void ChangeMontage();
	void ChangeMontage_Implementation();

	UFUNCTION(Reliable, NetMulticast)
	void ClientChangeMontage();
	void ClientChangeMontage_Implementation();

	// == Client ==
private :	
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision);
	
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapEnd();
	
public :
	// == ��Ī ���� �Լ� ==
	UFUNCTION()
	void ChangePOV();
};
