#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDProjectile.generated.h"

class UStaticMeshComponent;
class ATDEnemy;

UCLASS()
class PUTIKON_API ATDProjectile : public AActor
{
	GENERATED_BODY()

public:
	ATDProjectile();

	virtual void Tick(float DeltaTime) override;

	void InitializeProjectile(
		ATDEnemy* TargetEnemy,
		float DamageAmount
	);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float MoveSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float HitDistance = 30.0f;

private:
	UPROPERTY()
	ATDEnemy* Target;

	float Damage = 0.0f;
};
