#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDBuildTree.generated.h"

UCLASS()
class PUTIKON_API ATDBuildTree : public AActor
{
	GENERATED_BODY()

public:
	ATDBuildTree();

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Build"
	)
	TSubclassOf<AActor> TowerClass;

	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Build"
	)
	int32 BuildCost = 100;

	UFUNCTION(
		BlueprintCallable,
		Category = "Build"
	)
	void BuildTower();
};