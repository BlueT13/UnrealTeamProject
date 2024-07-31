// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Global/ContentsEnum.h"
#include "TestCharacter.generated.h"

// Inventory (for UI Test) => ����ĳ���ͷ� �����ؾ� �� (PickUpItem �Լ��� �ʿ�)
USTRUCT(BlueprintType)
struct FItemInformation
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FName Name = "";

	// Bullet Count, Damage
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int ReloadMaxNum = -1;	// �ѱ�� ���� ź �ִ� ����
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int ReloadLeftNum = -1;	// �ѱ�� ���� ź ���� ����
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	int Damage = 0;			// ���� ���ݷ�

	// Static Mesh
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMesh* MeshRes = nullptr;			// ����ƽ �޽� ���ҽ�
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector RelLoc = FVector(0.0f, 0.0f, 0.0f);		// ����ƽ �޽� ������Ʈ ����� ��ġ
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FRotator RelRot = FRotator(0.0f, 0.0f, 0.0f);	// ����ƽ �޽� ������Ʈ ����� ȸ��
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	FVector RelScale = FVector(1.0f, 1.0f, 1.0f);	// ����ƽ �޽� ������Ʈ ����� ũ��
};

UCLASS()
class UNREAL5_PORTFOLIO_API ATestCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestCharacter();

	// Components
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent = nullptr;
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent = nullptr;
	UPROPERTY(Category = "Contents", VisibleDefaultsOnly)
	USkeletalMeshComponent* FPVMesh = nullptr;	// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RidingMesh = nullptr;	// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���) [���ϴ� Component?] [Ż�� �׽��ÿ� (����� ��û)]
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemSocketMesh = nullptr;	// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FPVItemSocketMesh = nullptr;	// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UTestMinimapIconComponent* MinimapIconComponent = nullptr;
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UHeadNameWidgetComponent* HeadNameComponent = nullptr;	// => �������� ���� �ʿ� (24.07.30 �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GetMapItemCollisonComponent = nullptr;
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UParticleSystemComponent* MuzzleParticleComponent = nullptr;

	// State, Posture
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerState StateValue = EPlayerState::Idle;
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerPosture PostureValue = EPlayerPosture::Barehand;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EPlayerPosture PrevPostureValue = EPlayerPosture::Barehand;		// => �������� ���� �ʿ� (24.07.31 �߰���)

	// LowerState (��ȯ)
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerLowerState LowerStateValue = EPlayerLowerState::Idle;

	// UpperState
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerUpperState UpperStateValue = EPlayerUpperState::Rifle_Idle;

	// Dir
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerMoveDir DirValue = EPlayerMoveDir::Forward;

	// 7/26 �߰� ��������
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsFaint = false;

	UFUNCTION(Reliable, Server)
	void ChangeState(EPlayerState _Type);
	void ChangeState_Implementation(EPlayerState _Type);
	UFUNCTION(Reliable, Server)
	void ChangePosture(EPlayerPosture _Type);
	void ChangePosture_Implementation(EPlayerPosture _Type);

	// LowerStateChange �Լ� (��ȯ)
	UFUNCTION(Reliable, Server)
	void ChangeLowerState(EPlayerLowerState _LowerState);
	void ChangeLowerState_Implementation(EPlayerLowerState _LowerState);

	// DirChange �Լ� (��ȯ)
	UFUNCTION(Reliable, Server)
	void ChangePlayerDir(EPlayerMoveDir _Dir);
	void ChangePlayerDir_Implementation(EPlayerMoveDir _Dir);

	// 7/26 �������·� �����ϴ� �Լ� (��ȯ)
	UFUNCTION(Reliable, Server)
	void ChangeIsFaint();
	void ChangeIsFaint_Implementation();

	// POV	// => const ���� ����ĳ���� ���� �ʿ� (24.07.29 �߰���) => ���� ����.
	const FVector CameraRelLoc = FVector(0.0f, 60.0f, 110.0f);
	const FVector FPVCameraRelLoc = FVector(0.0f, 0.0f, 72.0f);
	const FVector FPVCameraRelLoc_Crouch = FVector(10.0f, 0.0f, 10.0f);
	bool IsFPV = true;
	UFUNCTION()
	void ChangePOV();
	UFUNCTION()
	void CharacterReload();

	// Inventory => ����ĳ���ͷ� �����ؾ� �� (PickUpItem �Լ��� �ʿ�)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FItemInformation> ItemSlot;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<bool> IsItemIn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurItemIndex = -1;

	// Item
	UPROPERTY(Category = "Contents", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* GetMapItemData = nullptr;	// �ʿ� �ִ� ���� Data
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	FString RayCastToItemName = "";
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapStart(AActor* _OtherActor, UPrimitiveComponent* _Collision);
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapEnd();
	UFUNCTION(BlueprintCallable)
	void CheckItem();	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)
	UFUNCTION(Reliable, Server)
	void InteractObject(AMapObjectBase* _MapObject);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)
	void InteractObject_Implementation(AMapObjectBase* _MapObject);
	UFUNCTION(Reliable, Server)
	void PickUpItem();
	void PickUpItem_Implementation();
	UFUNCTION(BlueprintCallable)
	void ItemSetting(FName _TagName, int _SlotIndex);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.30 �߰���)
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void DropItem(int _SlotIndex);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.30 ������)
	void DropItem_Implementation(int _SlotIndex);
	UFUNCTION(BlueprintCallable)
	void DeleteItem(int _Index);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)

	// Collision
	//UFUNCTION(BlueprintCallable)
	//void Collision(AActor* _OtherActor, UPrimitiveComponent* _Collision);

	// NotifyState���� ��� �� (��ȯ)
	UFUNCTION(BlueprintCallable)
	void HandAttackCollision(AActor* _OtherActor, UPrimitiveComponent* _Collision);

	// Notify���� ȣ��.
	void ChangeHandAttackCollisionProfile(FName _Name);

	UFUNCTION()
	void SendTokenToHpBarWidget();

	UFUNCTION(BlueprintCallable)
	void UpdatePlayerHp(float _DeltaTime);

	UFUNCTION(Reliable, Server)
	void SettingPlayerState();
	void SettingPlayerState_Implementation();

	UFUNCTION(BlueprintCallable)
	void ShowMuzzle();

	UFUNCTION(BlueprintCallable)
	void ShowEndMuzzle();

protected:
	// ���� �÷��̾� �߰� �ʿ� �ڵ� (��ȯ) 07/24
	void PostInitializeComponents() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// Item
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool PickUp = false;

private:
	// ���� ���ݿ� ��� �� (��ȯ)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* HandAttackComponent = nullptr;

	// ��Ÿ�� ���濡 ��� �� (��ȯ)
	UPROPERTY()
	class UPlayerAnimInstance* PlayerAnimInst;
	UPROPERTY()
	class UPlayerAnimInstance* FPVPlayerAnimInst;

public:
	// Fire
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void FireRayCast();			// => ���ε� �����ؾ� �� (24.07.25 ������)
	void FireRayCast_Implementation();

	// Drink
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void Drink();				// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
	void Drink_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void DrinkComplete();		// => ���ο� ���� �����ؾ� �� (24.07.30 �߰� �� �׽��� ��)
	void DrinkComplete_Implementation();

	// Bomb Setting
	UPROPERTY(Replicated ,BlueprintReadWrite)
	bool IsBombSetting = false;	// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetStart();		// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
	void BombSetStart_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetTick();			// => ���ο� ���� �����ؾ� �� (24.07.30 �߰� �� �׽��� ��)
	void BombSetTick_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetCancel();		// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
	void BombSetCancel_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetComplete();		// => ���ο� ���� �����ؾ� �� (24.07.29 �߰� �� �׽��� ��)
	void BombSetComplete_Implementation();

	// ���� �� ���� ĳ���� ��Ÿ�� ���� �Լ� (��ȯ)
	UFUNCTION(Reliable, Server)
	void ChangeMontage(bool _IsFireEnd);
	void ChangeMontage_Implementation(bool _IsFireEnd);

	// ���� �� Ŭ���̾�Ʈ ĳ���� ��Ÿ�� ���� �Լ� (��ȯ)
	UFUNCTION(Reliable, NetMulticast)
	void ClientChangeMontage(bool _IsFireEnd);
	void ClientChangeMontage_Implementation(bool _IsFireEnd);

	// Crouch ī�޶� �̵�
	UFUNCTION()
	void CrouchCameraMove();

	UFUNCTION()
	void NetCheck();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsServer = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsClient = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool IsCanControlled = false;

	UPROPERTY(Category = "TPSNet", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int Token = -1;
};