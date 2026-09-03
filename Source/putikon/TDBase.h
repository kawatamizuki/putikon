#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class PUTIKON_API ATDBase : public AActor
{
	GENERATED_BODY()

public:
	ATDBase();

	UFUNCTION(BlueprintCallable, Category = "Base")
	void EnemyReachedGoal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Base"
	)
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Base"
	)
	int32 RemainingChildren = 3;

	// Remaining Children‚ª•Ï‚í‚Á‚½Žž
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Base"
	)
	void OnChildrenChanged(int32 NewChildrenCount);

	// “G‚ª‹’“_‚Ö“ž’B‚µ‚½Žž
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Audio"
	)
	void OnBaseDamaged();

	// Game OverŽž
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Base"
	)
	void OnGameOver();
};