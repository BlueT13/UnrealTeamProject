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

	/// <summary>
	/// Component �ʱ�ȭ ���� ȣ��.
	/// </summary>
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

	// ��ü ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerUpperState UpperStateValue = EPlayerUpperState::UArm_Idle;
	
	// �÷��̾� �ڼ� ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerPosture PostureValue = EPlayerPosture::Barehand;
	
	// ĳ���� ���� ����
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerMoveDir DirValue = EPlayerMoveDir::Forward;

	// ĳ���� ���� ����.
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsFaint = false;

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

	// ��ź ��ġ ���� ��Ȳ
	UPROPERTY()
	bool IsBombSetting = false;


	// �ʿ� �ִ� ���� Data
	UPROPERTY(Category = "Contents", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* GetMapItemData = nullptr;

	// ��ü ����
	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInst = nullptr;
	UPROPERTY()
	class UPlayerAnimInstance* FPVPlayerAnimInst = nullptr;

	// ���� ���ݿ� ���
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HandAttackComponent = nullptr;

	UPROPERTY(Replicated)
	FName UIToSelectCharacter = "";

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerUpperState IdleDefault = EPlayerUpperState::UArm_Idle;

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

	UFUNCTION(BlueprintCallable)
	void ItemSetting(FName _TagName, bool _InNextSlotToItem);

	// Fire Ray Cast
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void FireRayCast(float _DeltaTime);
	void FireRayCast_Implementation(float _DeltaTime);

	UFUNCTION(Reliable, Server)
	void ChangeMontage(EPlayerUpperState _UpperState, bool IsSet = false);
	void ChangeMontage_Implementation(EPlayerUpperState _UpperState, bool IsSet = false);

	UFUNCTION(Reliable, NetMulticast)
	void ClientChangeMontage(EPlayerUpperState _UpperState);
	void ClientChangeMontage_Implementation(EPlayerUpperState _UpperState);

	UFUNCTION(Reliable, Server)
	void SettingPlayerState();
	void SettingPlayerState_Implementation();

	UFUNCTION(Reliable, Server)
	void ChangeIsFaint();
	void ChangeIsFaint_Implementation();

	UFUNCTION(Reliable, Server)
	void InteractObject(AMapObjectBase* _MapObject);
	void InteractObject_Implementation(AMapObjectBase* _MapObject);

	UFUNCTION(Reliable, Server)
	void BombSetStart();
	void BombSetStart_Implementation();
	UFUNCTION(Reliable, Server)
	void BombSetTick();
	void BombSetTick_Implementation();
	UFUNCTION(Reliable, Server)
	void BombSetCancel();
	void BombSetCancel_Implementation();
	UFUNCTION(Reliable, Server)
	void BombSetEnd();
	void BombSetEnd_Implementation();

	/// <summary>
	/// Crouch �� ���� ī�޶� �̵�
	/// </summary>
	UFUNCTION()
	void CrouchCameraMove();

	// == Client ==
private :
	const FVector CameraRelLoc = FVector(0.0f, 60.0f, 110.0f);
	const FVector FPVCameraRelLoc = FVector(0.0f, 0.0f, 72.0f);
	const FVector FPVCameraRelLoc_Crouch = FVector(10.0f, 0.0f, 10.0f);

	UFUNCTION(BlueprintCallable)
	void MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision);
	
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapEnd();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHp(float _DeltaTime);

	UFUNCTION(BlueprintCallable)
	void DeleteItem(int _Index);

	UFUNCTION(Reliable, Server, BlueprintCallable)
	void GetSetSelectCharacter(class UMainGameInstance* _MainGameInstance);
	void GetSetSelectCharacter_Implementation(class UMainGameInstance* _MainGameInstance);

	UFUNCTION(Reliable, Server)
	void DestroyItem(AItemBase* _Item);
	void DestroyItem_Implementation(AItemBase* _Item);

	UFUNCTION(Reliable, Server)
	void SetItemSocketVisibility(bool _Visibility);
	void SetItemSocketVisibility_Implementation(bool _Visibility);

	UFUNCTION(Reliable, Server)
	void SetItemSocketMesh(UStaticMesh* _ItemMeshRes, FVector _ItemRelLoc, FRotator _ItemRelRot, FVector _ItemRelScale);
	void SetItemSocketMesh_Implementation(UStaticMesh* _ItemMeshRes, FVector _ItemRelLoc, FRotator _ItemRelRot, FVector _ItemRelScale);

	// ���Ͽ� ������ ����.
	UFUNCTION()
	void SettingItemSocket(int _InputKey);

	// ������ ����
	UFUNCTION(BlueprintCallable)
	void PickUpItem(class AItemBase* _Item);

	// ������ ���� -> ���
	UFUNCTION(BlueprintCallable)
	void DropItem(int _SlotIndex);

	UFUNCTION()
	void DeleteItemInfo(int _Index);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsServer = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsClient = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsCanControlled = false;

	UPROPERTY(Category = "PlayerNet", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Token = -1;

	// == Client ==
public :
	UFUNCTION(BlueprintCallable)
	void CheckItem();

	UFUNCTION()
	void AttackCheck();

	UFUNCTION()
	void Drink();

	UFUNCTION()
	FORCEINLINE EPlayerUpperState GetIdleDefault() const
	{
		return IdleDefault;
	}

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

	// ĳ���� ���(�κ��丮) ����.
	UFUNCTION(BlueprintCallable)
	TArray<struct FPlayerItemInformation> GetItemSlot();

protected :
	UPROPERTY(Category = "Widget", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UGetItem_UserWidget* Reload_Widget = nullptr;
};

/** BP
* MovementComponent -> WalkableFloorAngle �� ���� 60.0���� ����.
* ĸ�� �ݸ��� Player�� ����.
*/