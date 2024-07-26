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

	void PostInitializeComponents() override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// AnimInstance
public :
	// ��ü ���� (Controller ���� ȣ����. -> ���߿� ���� �ʿ�.)
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerLowerState LowerStateValue = EPlayerLowerState::Idle;
	
	// �÷��̾� �ڼ� ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerPosture PostureValue = EPlayerPosture::Barehand;
	
	// ĳ���� ���� ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerMoveDir DirValue = EPlayerMoveDir::Forward;

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
	// Ż ��?
	//UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UStaticMeshComponent* RidingMesh = nullptr;
	// �̴ϸ� ������
	//UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	//class UTestMinimapIconComponent* MinimapIconComponent = nullptr;
	// �ʿ� �ִ� ������ Ž�� ���� �ݸ���
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GetMapItemCollisonComponent = nullptr;
	// ������ ������ ���� ��ġ
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* CreateItemComponent = nullptr;

	// ������ ���� ����
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemSocketMesh = nullptr;
	// 1��Ī ������ ���� ����
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FPVItemSocketMesh = nullptr;	// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)

		
	// == ��Ī ���� ���� ==
	UPROPERTY()
	bool IsFPV = true;

	// == Inventory ==
	// ������ ����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<bool> IsItemIn;
	// ���� ������ ����.
	UPROPERTY(VisibleAnywhere)
	TArray<struct FPlayerItemInformation> ItemSlot;
	// ���� ������ Index
	UPROPERTY(VisibleAnywhere)
	int CurItemIndex = -1;


	// �ʿ� �ִ� ���� Data
	UPROPERTY(Category = "Contents", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* GetMapItemData = nullptr;

	// ��ü ����
	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInst;
	UPROPERTY()
	class UPlayerAnimInstance* FPVPlayerAnimInst;

	// ���� ���ݿ� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HandAttackComponent = nullptr;


	// == Server ==
public :
	// ��ä ����
	UFUNCTION(Reliable, Server)
	void ChangePosture(EPlayerPosture _Type);
	void ChangePosture_Implementation(EPlayerPosture _Type);

	// ���� ����
	UFUNCTION(Reliable, Server)
	void ChangeLowerState(EPlayerLowerState _LowerState);
	void ChangeLowerState_Implementation(EPlayerLowerState _LowerState);

	// ĳ���� ����
	UFUNCTION(Reliable, Server)
	void ChangePlayerDir(EPlayerMoveDir _Dir);
	void ChangePlayerDir_Implementation(EPlayerMoveDir _Dir);

	// ������ ����
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void PickUpItem();
	void PickUpItem_Implementation();

	// ������ ���� -> ���
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void CharacterPlayerToDropItem();
	void CharacterPlayerToDropItem_Implementation();

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

	/// <summary>
	/// Crouch �� ���� ī�޶� �̵�
	/// </summary>
	UFUNCTION()
	void CrouchCameraMove();

	// == Client ==
private :	
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision);
	
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapEnd();

	UFUNCTION()
	void UpdatePlayerHp(float _DeltaTime);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsServer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsClient = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsCanControlled = false;

	UPROPERTY(Category = "PlayerNet", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Token = -1;

public :
	// == ��Ī ���� �Լ� ==
	UFUNCTION()
	void ChangePOV();

	// ���ε�.
	UFUNCTION()
	void CharacterReload();

	UFUNCTION(BlueprintCallable)
	void HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision);

	// Notify State���� ȣ��.
	UFUNCTION()
	void ChangeHandAttackCollisionProfile(FName _Name);

	UFUNCTION()
	void NetCheck();

	UFUNCTION()
	void SendTokenToHpBarWidget();
};

/** BP
* MovementComponent -> WalkableFloorAngle �� ���� 60.0���� ����.
*/