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

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* SpawnedTower = World->SpawnActor<AActor>(
		TowerClass,
		GetActorTransform(),
		SpawnParams
	);

	if (SpawnedTower)
	{
		Destroy();
	}
}
