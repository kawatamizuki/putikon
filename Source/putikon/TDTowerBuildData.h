#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "TDTowerBuildData.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PUTIKON_API UTDTowerBuildData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// UIに表示する名前
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	FText DisplayName;

	// UIに表示する説明
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	FText Description;

	// 建築するタワー
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	TSubclassOf<AActor> TowerClass;

	// 建築費
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	int32 Cost = 100;

	// UI用アイコン
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Tower"
	)
	UTexture2D* Icon = nullptr;
};