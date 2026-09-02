#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDEnemy.generated.h"

class UStaticMeshComponent;
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

	// SplineÇÇ«Ç±Ç‹Ç≈êiÇÒÇ≈Ç¢ÇÈÇ©éÊìæ
	float GetDistanceAlongSpline() const
	{
		return DistanceAlongSpline;
	}

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy")
	UStaticMeshComponent* EnemyMesh;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Movement",
		meta = (ExposeOnSpawn = "true")
	)
	AActor* PathActor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Enemy Stats")
	float CurrentHealth = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float ArrowDamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Stats")
	float CannonDamageMultiplier = 1.0f;

private:
	USplineComponent* PathSpline;

	float DistanceAlongSpline = 0.0f;

public:
	virtual void Tick(float DeltaTime) override;
};