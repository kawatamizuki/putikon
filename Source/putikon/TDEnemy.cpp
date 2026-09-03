#include "TDEnemy.h"

#include "TDBase.h"

#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

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
	}

	if (!PathSpline)
	{
		return;
	}

	// 親だけがCrowdメンバーを増やす
	if (
		SpawnMode == ETDEnemySpawnMode::Crowd &&
		!bIsCrowdMember &&
		ColumnCount > 1
		)
	{
		SetupCrowdFormation();
	}

	UpdateLocationOnSpline();
}

void ATDEnemy::SetupCrowdFormation()
{
	const int32 ActualColumnCount =
		FMath::Clamp(
			ColumnCount,
			1,
			4
		);

	const float CenterIndex =
		(static_cast<float>(ActualColumnCount) - 1.0f)
		* 0.5f;

	// 元々Spawnされた敵を1人目として使う
	const float FirstBaseOffset =
		(0.0f - CenterIndex)
		* ColumnSpacing;

	PathSideOffset =
		FirstBaseOffset +
		FMath::FRandRange(
			-SideRandomness,
			SideRandomness
		);

	DistanceAlongSpline =
		FMath::FRandRange(
			0.0f,
			ForwardRandomness
		);

	// 残りを同時に生成
	for (
		int32 ColumnIndex = 1;
		ColumnIndex < ActualColumnCount;
		++ColumnIndex
		)
	{
		const float BaseSideOffset =
			(
				static_cast<float>(ColumnIndex)
				- CenterIndex
				)
			* ColumnSpacing;

		const float SideOffset =
			BaseSideOffset +
			FMath::FRandRange(
				-SideRandomness,
				SideRandomness
			);

		const float ForwardOffset =
			FMath::FRandRange(
				0.0f,
				ForwardRandomness
			);

		SpawnCrowdMember(
			SideOffset,
			ForwardOffset
		);
	}
}

void ATDEnemy::SpawnCrowdMember(
	float SideOffset,
	float ForwardOffset
)
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	FActorSpawnParameters SpawnParams;

	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	SpawnParams.bDeferConstruction = true;

	ATDEnemy* NewEnemy =
		World->SpawnActor<ATDEnemy>(
			GetClass(),
			GetActorTransform(),
			SpawnParams
		);

	if (!NewEnemy)
	{
		return;
	}

	NewEnemy->PathActor =
		PathActor;

	NewEnemy->bIsCrowdMember =
		true;

	NewEnemy->PathSideOffset =
		SideOffset;

	NewEnemy->DistanceAlongSpline =
		ForwardOffset;

	NewEnemy->FinishSpawning(
		GetActorTransform()
	);
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

	for (
		int32 i = 0;
		i < CoinCount;
		++i
		)
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
		)
		* BobHeight;

	FVector NewRelativeLocation =
		InitialMeshRelativeLocation;

	NewRelativeLocation.Z +=
		BobOffset;

	VisualSkeletalMesh->SetRelativeLocation(
		NewRelativeLocation
	);

	const float Sway =
		FMath::Sin(
			AnimationTime * SwaySpeed
		)
		* SwayAngle;

	FRotator NewRelativeRotation =
		InitialMeshRelativeRotation;

	NewRelativeRotation.Yaw +=
		Sway;

	VisualSkeletalMesh->SetRelativeRotation(
		NewRelativeRotation
	);
}

void ATDEnemy::UpdateLocationOnSpline()
{
	if (!PathSpline)
	{
		return;
	}

	const float SplineLength =
		PathSpline->GetSplineLength();

	const float SafeDistance =
		FMath::Clamp(
			DistanceAlongSpline,
			0.0f,
			SplineLength
		);

	const FVector CenterLocation =
		PathSpline->GetLocationAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	const FVector RightVector =
		PathSpline->GetRightVectorAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	const FVector MoveDirection =
		PathSpline->GetDirectionAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	const FVector NewLocation =
		CenterLocation +
		RightVector * PathSideOffset;

	FRotator NewRotation =
		MoveDirection.Rotation();

	NewRotation.Pitch = 0.0f;
	NewRotation.Roll = 0.0f;
	NewRotation.Yaw += FacingYawOffset;

	SetActorLocationAndRotation(
		NewLocation,
		NewRotation
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

		Destroy();

		return;
	}

	UpdateLocationOnSpline();

	UpdateVisualAnimation(
		DeltaTime
	);
}