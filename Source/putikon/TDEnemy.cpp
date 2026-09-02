#include "TDEnemy.h"

#include "TDBase.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"

ATDEnemy::ATDEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("EnemyMesh")
		);

	RootComponent = EnemyMesh;

	PathActor = nullptr;
	PathSpline = nullptr;
}

void ATDEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	if (PathActor)
	{
		PathSpline =
			PathActor->FindComponentByClass<
			USplineComponent
			>();

		if (PathSpline)
		{
			const FVector StartLocation =
				PathSpline
				->GetLocationAtDistanceAlongSpline(
					0.0f,
					ESplineCoordinateSpace::World
				);

			SetActorLocation(
				StartLocation
			);
		}
	}
}

void ATDEnemy::ReceiveArrowDamage(
	float DamageAmount
)
{
	if (bDead)
	{
		return;
	}

	const float FinalDamage =
		DamageAmount *
		ArrowDamageMultiplier;

	CurrentHealth -=
		FinalDamage;

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void ATDEnemy::ReceiveCannonDamage(
	float DamageAmount
)
{
	if (bDead)
	{
		return;
	}

	const float FinalDamage =
		DamageAmount *
		CannonDamageMultiplier;

	CurrentHealth -=
		FinalDamage;

	if (CurrentHealth <= 0.0f)
	{
		Die();
	}
}

void ATDEnemy::Die()
{
	if (bDead)
	{
		return;
	}

	bDead = true;

	DropCoins();

	Destroy();
}

void ATDEnemy::DropCoins()
{
	if (!CoinClass)
	{
		return;
	}

	UWorld* World =
		GetWorld();

	if (!World)
	{
		return;
	}

	const int32 RandomOffset =
		FMath::RandRange(
			-MoneyRandomRange,
			MoneyRandomRange
		);

	const int32 CoinCount =
		FMath::Max(
			BaseMoneyReward +
			RandomOffset,
			0
		);

	const FVector EnemyLocation =
		GetActorLocation();

	for (int32 i = 0; i < CoinCount; ++i)
	{
		FVector SpawnLocation =
			EnemyLocation;

		// 少しばらけさせる
		SpawnLocation.X +=
			FMath::FRandRange(
				-35.0f,
				35.0f
			);

		SpawnLocation.Y +=
			FMath::FRandRange(
				-35.0f,
				35.0f
			);

		SpawnLocation.Z +=
			FMath::FRandRange(
				20.0f,
				50.0f
			);

		FActorSpawnParameters SpawnParams;

		// コイン側で「落とした敵」を無視できるようにする
		SpawnParams.Owner = this;

		SpawnParams
			.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod
			::AlwaysSpawn;

		World->SpawnActor<AActor>(
			CoinClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);
	}
}

void ATDEnemy::Tick(
	float DeltaTime
)
{
	Super::Tick(
		DeltaTime
	);

	if (!PathSpline)
	{
		return;
	}

	if (bDead)
	{
		return;
	}

	DistanceAlongSpline +=
		MoveSpeed *
		DeltaTime;

	const float SplineLength =
		PathSpline->GetSplineLength();

	if (
		DistanceAlongSpline >=
		SplineLength
		)
	{
		AActor* BaseActor =
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				ATDBase::StaticClass()
			);

		ATDBase* Base =
			Cast<ATDBase>(
				BaseActor
			);

		if (Base)
		{
			Base->EnemyReachedGoal();
		}

		// ゴール到達ではコインを落とさない
		Destroy();

		return;
	}

	const FVector NewLocation =
		PathSpline
		->GetLocationAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

	const FRotator NewRotation =
		PathSpline
		->GetRotationAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

	SetActorLocationAndRotation(
		NewLocation,
		NewRotation
	);
}