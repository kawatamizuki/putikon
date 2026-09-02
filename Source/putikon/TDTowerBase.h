#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDTowerBase.generated.h"

class USphereComponent;
class USceneComponent;
class UStaticMeshComponent;
class ATDEnemy;
class ATDProjectile;
class ATDCannonProjectile;

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

	// =========================
	// Components
	// =========================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	USceneComponent* Root;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	USceneComponent* TowerRotationRoot;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	USphereComponent* AttackRange;

	// =========================
	// Common Tower Settings
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float AttackInterval = 1.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float AttackDamage = 10.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float Range = 500.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	ETowerAttackType AttackType =
		ETowerAttackType::Arrow;

	// =========================
	// Arrow
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Arrow"
	)
	TSubclassOf<ATDProjectile> ProjectileClass;

	// =========================
	// Cannon
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	TSubclassOf<ATDCannonProjectile>
		CannonProjectileClass;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	float CannonProjectileSpeed = 500.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	float CannonArcHeight = 150.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	float CannonExplosionRadius = 300.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	float CannonMinDamagePercent = 0.5f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Cannon"
	)
	float CannonShockwaveDuration = 0.35f;

	// =========================
	// Projectile Spawn
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float ProjectileSpawnHeight = 100.0f;

	// =========================
	// Rotation
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float AimYawOffset = 0.0f;

	// =========================
	// Range Display
	// =========================

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Tower"
	)
	float TouchMargin = 70.0f;

	// =========================
	// Runtime
	// =========================

	UPROPERTY()
	ATDEnemy* CurrentTarget;

	UPROPERTY()
	UStaticMeshComponent* TowerMeshComponent;

	FTimerHandle AttackTimerHandle;

	// =========================
	// Functions
	// =========================

	void FindTarget();

	void Attack();

	void RotateTowardTarget();

	bool IsPlayerTouchingTower() const;
};