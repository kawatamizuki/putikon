#include "TDBuildTree.h"

#include "TDTowerBuildData.h"
#include "TDPlayerMoneyComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ATDBuildTree::ATDBuildTree()
{
	PrimaryActorTick.bCanEverTick = false;
}

int32 ATDBuildTree::GetTowerOptionCount() const
{
	return TowerOptions.Num();
}

UTDTowerBuildData* ATDBuildTree::GetTowerOption(
	int32 Index
) const
{
	if (!TowerOptions.IsValidIndex(Index))
	{
		return nullptr;
	}

	return TowerOptions[Index];
}

bool ATDBuildTree::BuildTowerByIndex(
	int32 Index
)
{
	if (!TowerOptions.IsValidIndex(Index))
	{
		return false;
	}

	UTDTowerBuildData* TowerData =
		TowerOptions[Index];

	if (!IsValid(TowerData))
	{
		return false;
	}

	if (!TowerData->TowerClass)
	{
		return false;
	}

	UWorld* World =
		GetWorld();

	if (!World)
	{
		return false;
	}

	ACharacter* Player =
		UGameplayStatics::GetPlayerCharacter(
			World,
			0
		);

	if (!IsValid(Player))
	{
		return false;
	}

	UTDPlayerMoneyComponent* MoneyComponent =
		Player->FindComponentByClass<
		UTDPlayerMoneyComponent
		>();

	if (!MoneyComponent)
	{
		return false;
	}

	// ‚¨‹à‚ğx•¥‚¤
	if (!MoneyComponent->SpendMoney(
		TowerData->Cost
	))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Not enough money.")
		);

		return false;
	}

	FVector SpawnLocation =
		GetActorLocation();

	// ’n–Ê‚ğ’T‚·
	const FVector TraceStart =
		SpawnLocation +
		FVector(
			0.0f,
			0.0f,
			500.0f
		);

	const FVector TraceEnd =
		SpawnLocation -
		FVector(
			0.0f,
			0.0f,
			1000.0f
		);

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;

	QueryParams.AddIgnoredActor(
		this
	);

	const bool bHit =
		World->LineTraceSingleByChannel(
			HitResult,
			TraceStart,
			TraceEnd,
			ECC_Visibility,
			QueryParams
		);

	if (bHit)
	{
		SpawnLocation =
			HitResult.Location;
	}

	FTransform SpawnTransform;

	SpawnTransform.SetLocation(
		SpawnLocation
	);

	SpawnTransform.SetRotation(
		FQuat(
			FRotator(
				0.0f,
				GetActorRotation().Yaw,
				0.0f
			)
		)
	);

	SpawnTransform.SetScale3D(
		FVector::OneVector
	);

	FActorSpawnParameters SpawnParams;

	SpawnParams
		.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod
		::AlwaysSpawn;

	AActor* SpawnedTower =
		World->SpawnActor<AActor>(
			TowerData->TowerClass,
			SpawnTransform,
			SpawnParams
		);

	if (!SpawnedTower)
	{
		// Spawn¸”s‚Í•Ô‹à
		MoneyComponent->AddMoney(
			TowerData->Cost
		);

		return false;
	}

	// Œšİ¬Œ÷
	OnTowerBuilt();

	Destroy();

	return true;
}