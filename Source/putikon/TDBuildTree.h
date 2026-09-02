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

	// この木に建設するタワー
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Build")
	TSubclassOf<AActor> TowerClass;

	// タワーを建設する
	UFUNCTION(BlueprintCallable, Category = "Build")
	void BuildTower();
};