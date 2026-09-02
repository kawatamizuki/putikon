#include "TDBuildTree.h"

ATDBuildTree::ATDBuildTree()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATDBuildTree::BuildTower()
{
	if (!TowerClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("TowerClass is not set."));
		return;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	// 木の位置を基準にする
	FVector SpawnLocation = GetActorLocation();

	// 上から下へLineTraceして地面の位置を取得
	FHitResult HitResult;

	const FVector TraceStart =
		SpawnLocation + FVector(0.0f, 0.0f, 500.0f);

	const FVector TraceEnd =
		SpawnLocation - FVector(0.0f, 0.0f, 1000.0f);

	FCollisionQueryParams QueryParams;

	// 木自身には当たらないようにする
	QueryParams.AddIgnoredActor(this);

	const bool bHit = World->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// 地面に当たった場合、その位置を使う
	if (bHit)
	{
		SpawnLocation = HitResult.Location;
	}

	// 木の回転・Scaleは引き継ぎ、位置だけ地面にする
	FTransform SpawnTransform = GetActorTransform();
	SpawnTransform.SetLocation(SpawnLocation);

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedTower = World->SpawnActor<AActor>(
		TowerClass,
		SpawnTransform,
		SpawnParams
	);

	// Spawn成功したら木を消す
	if (SpawnedTower)
	{
		Destroy();
	}
}
