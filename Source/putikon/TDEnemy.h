#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDEnemy.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class USplineComponent;

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

	virtual void Tick(float DeltaTime) override;

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
		Category = "Movement"
	)
	float FacingYawOffset = 0.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Animation"
	)
	float BobHeight = 8.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Animation"
	)
	float BobSpeed = 7.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Animation"
	)
	float SwayAngle = 8.0f;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Animation"
	)
	float SwaySpeed = 7.0f;

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

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	TSubclassOf<AActor> CoinClass;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	int32 BaseMoneyReward = 10;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Reward"
	)
	int32 MoneyRandomRange = 3;

	// “GŒ‚”jŽž
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Audio"
	)
	void OnEnemyDefeated();

private:
	USplineComponent* PathSpline = nullptr;

	USkeletalMeshComponent* VisualSkeletalMesh = nullptr;

	FVector InitialMeshRelativeLocation = FVector::ZeroVector;
	FRotator InitialMeshRelativeRotation = FRotator::ZeroRotator;

	float DistanceAlongSpline = 0.0f;
	float AnimationTime = 0.0f;

	bool bDead = false;

	void Die();
	void DropCoins();
	void UpdateVisualAnimation(float DeltaTime);
};