#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDTowerBase.generated.h"

class USphereComponent;
class USceneComponent;
class UStaticMeshComponent;
class ATDEnemy;
class ATDProjectile;

UENUM(BlueprintType)
enum class ETowerAttackType : uint8
{
	Arrow,
	Cannon
};

UCLASS()
class PUTIKON_API ATDTowerBase : public AActor
{
	GENERATED_BODY()

public:
	ATDTowerBase();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	USceneComponent* TowerRotationRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	USphereComponent* AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AttackInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float Range = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	ETowerAttackType AttackType = ETowerAttackType::Arrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	TSubclassOf<ATDProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float ProjectileSpawnHeight = 100.0f;

	// モデルの正面方向補正
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AimYawOffset = 0.0f;

	// タワーにどれくらい近づいたら
	// 「触れている」と判定するか
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float TouchMargin = 70.0f;

	UPROPERTY()
	ATDEnemy* CurrentTarget;

	UPROPERTY()
	UStaticMeshComponent* TowerMeshComponent;

	FTimerHandle AttackTimerHandle;

	void FindTarget();

	void Attack();

	void RotateTowardTarget();

	bool IsPlayerTouchingTower() const;
};