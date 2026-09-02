#include "TDBuildTree.h"
#include "TDPlayerMoneyComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ATDBuildTree::ATDBuildTree()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ATDBuildTree::BuildTower()
{
	if (!TowerClass)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("TowerClass is not set.")
		);

		return;
	}

	UWorld* World =
		GetWorld();

	if (!World)
	{
		return;
	}

	// プレイヤー取得
	ACharacter* Player =
		UGameplayStatics::GetPlayerCharacter(
			World,
			0
		);

	if (!IsValid(Player))
	{
		return;
	}

	// MoneyComponent取得
	UTDPlayerMoneyComponent* MoneyComponent =
		Player->FindComponentByClass<
		UTDPlayerMoneyComponent
		>();

	if (!MoneyComponent)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("TDPlayerMoneyComponent not found.")
		);

		return;
	}

	// お金が足りるか確認
	if (!MoneyComponent->SpendMoney(
		BuildCost
	))
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Not enough money.")
		);

		return;
	}

	FVector SpawnLocation =
		GetActorLocation();

	// 地面を探す
	FHitResult HitResult;

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
			TowerClass,
			SpawnTransform,
			SpawnParams
		);

	if (SpawnedTower)
	{
		Destroy();
	}
	else
	{
		// Spawn失敗したら返金
		MoneyComponent->AddMoney(
			BuildCost
		);
	}
}
