// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Global/ContentsEnum.h"
#include "TestFPVCharacter.generated.h"

// For Character Part's Testing : First person point of view 

// Inventory (for UI Test) => ����ĳ���ͷ� �����ؾ� �� (PickUpItem �Լ��� �ʿ�)
USTRUCT(BlueprintType)
struct FFPVItemInformation
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
class UNREAL5_PORTFOLIO_API ATestFPVCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATestFPVCharacter();

	// Components
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* SpringArmComponent = nullptr;
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* CameraComponent = nullptr;
	UPROPERTY(Category = "Contents", VisibleDefaultsOnly)
	USkeletalMeshComponent* FPVMesh = nullptr;						// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* RidingMesh = nullptr;				// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���) [���ϴ� Component?] [Ż�� �׽��ÿ� (����� ��û)]
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* ItemSocketMesh = nullptr;			// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UStaticMeshComponent* FPVItemSocketMesh = nullptr;		// => ����ĳ���ͷ� �����ؾ� �� (���� �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UTestMinimapIconComponent* MinimapIconComponent = nullptr;
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UHeadNameWidgetComponent* HeadNameComponent = nullptr;	// => �������� ���� �ʿ� (24.07.30 �߰���)
	UPROPERTY(Category = "Contents", EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* GetMapItemCollisionComponent = nullptr;	// => ���� ���� �ʿ� (24.08.01 ��Ÿ ������)

	// Posture
	UPROPERTY(Category = "Contents", Replicated, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	EPlayerPosture PostureValue = EPlayerPosture::Barehand;
	UPROPERTY(Category = "Contents", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
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

	// POV
	const FVector CameraRelLoc = FVector(0.0f, 60.0f, 110.0f);			// => ����ĳ���� ���� �ʿ� (24.07.29 �߰���) => ���� ����.
	const FVector FPVCameraRelLoc = FVector(10.0f, 0.0f, 72.0f);		// => ���� ���� �ʿ� (24.08.02 ������)
	const FVector FPVCameraRelLoc_Crouch = FVector(10.0f, 0.0f, 10.0f);	// => ����ĳ���� ���� �ʿ� (24.07.29 �߰���) => ���� ����.
	//bool IsFPV = true;
	EPlayerFPSTPSState PointOfView = EPlayerFPSTPSState::FPS;
	UFUNCTION()
	void ChangePOV();
	UFUNCTION()
	void CharacterReload();

	// Inventory => ����ĳ���ͷ� �����ؾ� �� (PickUpItem �Լ��� �ʿ�)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FFPVItemInformation> ItemSlot;
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
	void MapItemOverlapStart(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);	// => ���� ���� �ʿ� (24.08.01 ������)
	UFUNCTION(BlueprintCallable)
	void MapItemOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);	// => ���� ���� �ʿ� (24.08.01 ������)
	UFUNCTION(BlueprintCallable)
	void CheckItem();									// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)
	UFUNCTION(Reliable, Server)
	void InteractObject(AMapObjectBase* _MapObject);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)
	void InteractObject_Implementation(AMapObjectBase* _MapObject);
	UFUNCTION(Reliable, Server)
	void PickUpItem(AActor* _Item);						// => ���� ���� �ʿ� (24.08.02 ���� �߰���)
	void PickUpItem_Implementation(AActor* _Item);
	UFUNCTION(BlueprintCallable)
	void ItemSetting(FName _TagName, int _SlotIndex);	// => ����ĳ���ͷ� �����ؾ� �� (24.07.30 �߰���)
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void DropItem(int _SlotIndex);						// => ����ĳ���ͷ� �����ؾ� �� (24.07.30 ������)
	void DropItem_Implementation(int _SlotIndex);
	UFUNCTION(BlueprintCallable)
	void DeleteItem(int _Index);						// => ����ĳ���ͷ� �����ؾ� �� (24.07.29 �߰���)

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
	void FireRayCast();
	void FireRayCast_Implementation();

	// Drink
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void Drink();
	void Drink_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void DrinkComplete();
	void DrinkComplete_Implementation();

	// Bomb Setting
	UPROPERTY(Replicated, BlueprintReadWrite)
	bool IsBombSetting = false;	// => ���ο� ���� �ʿ� (24.07.29 �߰���)
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetStart();		// => ���ο� ���� �ʿ� (24.07.31 ������)
	void BombSetStart_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetTick();			// => ���ο� ���� �ʿ� (24.07.31 ������)
	void BombSetTick_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetCancel();		// => ���ο� ���� �ʿ� (24.07.31 ������)
	void BombSetCancel_Implementation();
	UFUNCTION(Reliable, Server, BlueprintCallable)
	void BombSetComplete();		// => ���ο� ���� �ʿ� (24.07.31 ������)
	void BombSetComplete_Implementation();

	// ���� �� ���� ĳ���� ��Ÿ�� ���� �Լ� (��ȯ)
	UFUNCTION(Reliable, Server)
	void ChangeMontage(EPlayerUpperState _UpperState);
	void ChangeMontage_Implementation(EPlayerUpperState _UpperState);
	UFUNCTION(Reliable, NetMulticast)
	void ClientChangeMontage(EPlayerUpperState _UpperState);
	void ClientChangeMontage_Implementation(EPlayerUpperState _UpperState);


	// Crouch ī�޶� �̵�
	UFUNCTION()
	void CrouchCameraMove();

	UFUNCTION()
	void AttackCheck();

	UFUNCTION()
	void AttackEndCheck();

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
