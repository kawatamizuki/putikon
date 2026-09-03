#include "TDEnemy.h"

#include "TDBase.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
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

	// BP‚Å’Ç‰Á‚µ‚½SkeletalMesh‚ð’T‚·
	VisualSkeletalMesh =
		FindComponentByClass<USkeletalMeshComponent>();

	if (VisualSkeletalMesh)
	{
		InitialMeshRelativeLocation =
			VisualSkeletalMesh->GetRelativeLocation();

		InitialMeshRelativeRotation =
			VisualSkeletalMesh->GetRelativeRotation();
	}

	if (PathActor)
	{
		PathSpline =
			PathActor->FindComponentByClass<USplineComponent>();

		if (PathSpline)
		{
			const FVector StartLocation =
				PathSpline->GetLocationAtDistanceAlongSpline(
					0.0f,
					ESplineCoordinateSpace::World
				);

			const FVector StartDirection =
				PathSpline->GetDirectionAtDistanceAlongSpline(
					0.0f,
					ESplineCoordinateSpace::World
				);

			FRotator StartRotation =
				StartDirection.Rotation();

			StartRotation.Pitch = 0.0f;
			StartRotation.Roll = 0.0f;
			StartRotation.Yaw += FacingYawOffset;

			SetActorLocationAndRotation(
				StartLocation,
				StartRotation
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

	CurrentHealth -= FinalDamage;

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

	CurrentHealth -= FinalDamage;

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

	// “GŒ‚”j‰¹—pƒCƒxƒ“ƒg
	OnEnemyDefeated();

	DropCoins();

	Destroy();
}

void ATDEnemy::DropCoins()
{
	if (!CoinClass)
	{
		return;
	}

	UWorld* World = GetWorld();

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
			BaseMoneyReward + RandomOffset,
			0
		);

	const FVector EnemyLocation =
		GetActorLocation();

	for (int32 i = 0; i < CoinCount; ++i)
	{
		FVector SpawnLocation =
			EnemyLocation;

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

		SpawnParams.Owner = this;

		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		World->SpawnActor<AActor>(
			CoinClass,
			SpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);
	}
}

void ATDEnemy::UpdateVisualAnimation(
	float DeltaTime
)
{
	if (!VisualSkeletalMesh)
	{
		return;
	}

	AnimationTime += DeltaTime;

	const float BobOffset =
		FMath::Sin(
			AnimationTime * BobSpeed
		) * BobHeight;

	FVector NewRelativeLocation =
		InitialMeshRelativeLocation;

	NewRelativeLocation.Z += BobOffset;

	VisualSkeletalMesh->SetRelativeLocation(
		NewRelativeLocation
	);

	const float Sway =
		FMath::Sin(
			AnimationTime * SwaySpeed
		) * SwayAngle;

	FRotator NewRelativeRotation =
		InitialMeshRelativeRotation;

	NewRelativeRotation.Yaw += Sway;

	VisualSkeletalMesh->SetRelativeRotation(
		NewRelativeRotation
	);
}

void ATDEnemy::Tick(
	float DeltaTime
)
{
	Super::Tick(DeltaTime);

	if (!PathSpline)
	{
		return;
	}

	if (bDead)
	{
		return;
	}

	DistanceAlongSpline +=
		MoveSpeed * DeltaTime;

	const float SplineLength =
		PathSpline->GetSplineLength();

	if (DistanceAlongSpline >= SplineLength)
	{
		AActor* BaseActor =
			UGameplayStatics::GetActorOfClass(
				GetWorld(),
				ATDBase::StaticClass()
			);

		ATDBase* Base =
			Cast<ATDBase>(BaseActor);

		if (Base)
		{
			Base->EnemyReachedGoal();
		}

		Destroy();

		return;
	}

	const FVector NewLocation =
		PathSpline->GetLocationAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

	const FVector MoveDirection =
		PathSpline->GetDirectionAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

	FRotator NewRotation =
		MoveDirection.Rotation();

	NewRotation.Pitch = 0.0f;
	NewRotation.Roll = 0.0f;
	NewRotation.Yaw += FacingYawOffset;

	SetActorLocationAndRotation(
		NewLocation,
		NewRotation
	);

	UpdateVisualAnimation(
		DeltaTime
	);
}