#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDEnemy.generated.h"

class UStaticMeshComponent;
class USplineComponent;
class ATDCoinPickup;

UCLASS()
class PUTIKON_API ATDEnemy : public AActor
{
	GENERATED_BODY()

public:
	ATDEnemy();

	UFUNCTION(BlueprintCallable, Category = "Enemy Stats")
	void ReceiveArrowDamage(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Enemy Stats")
	void ReceiveCannonDamage(float DamageAmount);

	float GetDistanceAlongSpline() const
	{
		return DistanceAlongSpline;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy"
	)
	UStaticMeshComponent* EnemyMesh;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Movement",
		meta = (ExposeOnSpawn = "true")
	)
	AActor* PathActor;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Movement"
	)
	float MoveSpeed = 200.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy Stats"
	)
	float MaxHealth = 100.0f;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Enemy Stats"
	)
	float CurrentHealth = 100.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy Stats"
	)
	float ArrowDamageMultiplier = 1.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Enemy Stats"
	)
	float CannonDamageMultiplier = 1.0f;

	// 落とすコイン
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	TSubclassOf<ATDCoinPickup> CoinClass;

	// 基本ドロップ枚数
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	int32 BaseMoneyReward = 10;

	// 基本値から±何枚するか
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	int32 MoneyRandomRange = 3;

private:
	USplineComponent* PathSpline;

	float DistanceAlongSpline = 0.0f;

	bool bDead = false;

	void Die();

	void DropCoins();

public:
	virtual void Tick(float DeltaTime) override;
};