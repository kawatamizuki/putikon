#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDCannonProjectile.generated.h"

class UStaticMeshComponent;

UCLASS()
class PUTIKON_API ATDCannonProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATDCannonProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitializeProjectile(
		const FVector& TargetLocation,
		float DamageAmount,
		float InMoveSpeed,
		float InArcHeight,
		float InExplosionRadius,
		float InMinDamagePercent,
		float InShockwaveDuration
	);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Cannon"
	)
	UStaticMeshComponent* ProjectileMesh;

	// ‘å–C’…’eŽž
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Audio"
	)
	void OnCannonImpact();

private:
	FVector StartLocation;
	FVector DestinationLocation;

	float BaseDamage = 200.0f;

	float MoveSpeed = 500.0f;

	float ArcHeight = 150.0f;

	float ExplosionRadius = 300.0f;

	float MinDamagePercent = 0.5f;

	float ShockwaveDuration = 0.35f;

	float TravelDistance = 0.0f;

	float TravelTime = 1.0f;

	float ElapsedTime = 0.0f;

	float ShockwaveElapsedTime = 0.0f;

	bool bExploded = false;

	void Explode();

	void UpdateShockwave(
		float DeltaTime
	);
};