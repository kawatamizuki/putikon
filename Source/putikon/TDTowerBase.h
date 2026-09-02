#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDTowerBase.generated.h"

class USphereComponent;
class ATDEnemy;

UCLASS()
class PUTIKON_API ATDTowerBase : public AActor
{
	GENERATED_BODY()

public:
	ATDTowerBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tower")
	USphereComponent* AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AttackInterval = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float AttackDamage = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float Range = 500.0f;

	UPROPERTY()
	ATDEnemy* CurrentTarget;

	FTimerHandle AttackTimerHandle;

	void FindTarget();
	void Attack();

public:
	UFUNCTION(BlueprintCallable, Category = "Tower")
	void StartAttack();
};