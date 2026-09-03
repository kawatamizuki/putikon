#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDBuildTree.generated.h"

class UTDTowerBuildData;

UCLASS()
class PUTIKON_API ATDBuildTree : public AActor
{
	GENERATED_BODY()

public:
	ATDBuildTree();

	// この木から建築できるタワー一覧
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Build"
	)
	TArray<UTDTowerBuildData*> TowerOptions;

	// 指定番号のタワーを建設
	UFUNCTION(
		BlueprintCallable,
		Category = "Build"
	)
	bool BuildTowerByIndex(int32 Index);

	// タワー候補数
	UFUNCTION(
		BlueprintPure,
		Category = "Build"
	)
	int32 GetTowerOptionCount() const;

	// 指定番号のデータを取得
	UFUNCTION(
		BlueprintPure,
		Category = "Build"
	)
	UTDTowerBuildData* GetTowerOption(int32 Index) const;

	// 建築成功時にBlueprint側で呼ばれる
	UFUNCTION(
		BlueprintImplementableEvent,
		Category = "Build"
	)
	void OnTowerBuilt();
};