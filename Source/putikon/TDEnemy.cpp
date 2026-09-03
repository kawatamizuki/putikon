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


// ========================================
// BeginPlay
// ========================================

void ATDEnemy::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	// BPで追加したSkeletalMeshを探す
	VisualSkeletalMesh =
		FindComponentByClass<USkeletalMeshComponent>();

	if (VisualSkeletalMesh)
	{
		InitialMeshRelativeLocation =
			VisualSkeletalMesh->GetRelativeLocation();

		InitialMeshRelativeRotation =
			VisualSkeletalMesh->GetRelativeRotation();
	}

	// PathActorからSpline取得
	if (PathActor)
	{
		PathSpline =
			PathActor->FindComponentByClass<USplineComponent>();
	}

	if (PathSpline)
	{
		UpdateLocationOnSpline();
	}
}


// ========================================
// Spawn Formation
// ========================================

void ATDEnemy::ConfigureSpawnFormation(
	bool bUseCrowdSpawn,
	int32 InColumnCount
)
{
	// 二重実行防止
	if (bFormationConfigured)
	{
		return;
	}

	bFormationConfigured = true;

	// Singleなら何もしない
	if (!bUseCrowdSpawn)
	{
		return;
	}

	const int32 ColumnCount =
		FMath::Clamp(
			InColumnCount,
			1,
			4
		);

	// 1ならSingleと同じ
	if (ColumnCount <= 1)
	{
		return;
	}

	if (!PathSpline)
	{
		if (PathActor)
		{
			PathSpline =
				PathActor->FindComponentByClass<USplineComponent>();
		}
	}

	if (!PathSpline)
	{
		return;
	}

	// ========================================
	// 横位置を中央基準にする
	//
	// 2体:
	// -0.5  +0.5
	//
	// 3体:
	// -1    0    +1
	//
	// 4体:
	// -1.5 -0.5 +0.5 +1.5
	// ========================================

	const float CenterIndex =
		(
			static_cast<float>(ColumnCount)
			- 1.0f
			)
		* 0.5f;

	// ========================================
	// 元々Spawnされた敵を
	// 1人目として利用
	// ========================================

	const float FirstBaseSideOffset =
		(
			0.0f
			- CenterIndex
			)
		* CrowdColumnSpacing;

	PathSideOffset =
		FirstBaseSideOffset +
		FMath::FRandRange(
			-CrowdSideRandomness,
			CrowdSideRandomness
		);

	// 前後に少しズラして
	// 完全な横一列にしない
	DistanceAlongSpline +=
		FMath::FRandRange(
			0.0f,
			CrowdForwardRandomness
		);

	UpdateLocationOnSpline();

	// ========================================
	// 残りの敵を同時Spawn
	// ========================================

	for (
		int32 ColumnIndex = 1;
		ColumnIndex < ColumnCount;
		++ColumnIndex
		)
	{
		const float BaseSideOffset =
			(
				static_cast<float>(
					ColumnIndex
					)
				- CenterIndex
				)
			* CrowdColumnSpacing;

		const float FinalSideOffset =
			BaseSideOffset +
			FMath::FRandRange(
				-CrowdSideRandomness,
				CrowdSideRandomness
			);

		const float ForwardOffset =
			FMath::FRandRange(
				0.0f,
				CrowdForwardRandomness
			);

		SpawnCrowdMember(
			FinalSideOffset,
			ForwardOffset
		);
	}
}


// ========================================
// Crowd Member Spawn
// ========================================

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

	// 親の位置・回転を使う
	FTransform SpawnTransform = GetActorTransform();

	// Spawn時はいったんScaleを1にする
	// BP側の見た目Scaleが二重に影響するのを防ぐ
	SpawnTransform.SetScale3D(
		FVector::OneVector
	);

	ATDEnemy* NewEnemy =
		World->SpawnActorDeferred<ATDEnemy>(
			GetClass(),
			SpawnTransform,
			nullptr,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		);

	if (!NewEnemy)
	{
		return;
	}

	NewEnemy->PathActor =
		PathActor;

	NewEnemy->PathSideOffset =
		SideOffset;

	NewEnemy->DistanceAlongSpline =
		DistanceAlongSpline +
		ForwardOffset;

	NewEnemy->bFormationConfigured =
		true;

	UGameplayStatics::FinishSpawningActor(
		NewEnemy,
		SpawnTransform
	);

	// 最後に親と同じActor Scaleにする
	NewEnemy->SetActorScale3D(
		GetActorScale3D()
	);
}


// ========================================
// Arrow Damage
// ========================================

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


// ========================================
// Cannon Damage
// ========================================

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


// ========================================
// Die
// ========================================

void ATDEnemy::Die()
{
	if (bDead)
	{
		return;
	}

	bDead = true;

	// 敵撃破音
	OnEnemyDefeated();

	DropCoins();

	Destroy();
}


// ========================================
// Drop Coins
// ========================================

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
			BaseMoneyReward +
			RandomOffset,
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


// ========================================
// Visual Animation
// ========================================

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
			AnimationTime *
			BobSpeed
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
			AnimationTime *
			SwaySpeed
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


// ========================================
// Spline位置更新
// ========================================

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

	// Spline中心
	const FVector CenterLocation =
		PathSpline->GetLocationAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	// Spline横方向
	const FVector RightVector =
		PathSpline->GetRightVectorAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	// 進行方向
	const FVector MoveDirection =
		PathSpline->GetDirectionAtDistanceAlongSpline(
			SafeDistance,
			ESplineCoordinateSpace::World
		);

	// 横Offsetを追加
	const FVector NewLocation =
		CenterLocation +
		RightVector *
		PathSideOffset;

	FRotator NewRotation =
		MoveDirection.Rotation();

	NewRotation.Pitch = 0.0f;
	NewRotation.Roll = 0.0f;
	NewRotation.Yaw +=
		FacingYawOffset;

	SetActorLocationAndRotation(
		NewLocation,
		NewRotation
	);
}


// ========================================
// Tick
// ========================================

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
		MoveSpeed *
		DeltaTime;

	const float SplineLength =
		PathSpline->GetSplineLength();

	// ========================================
	// Goal
	// ========================================

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

	// Spline移動
	UpdateLocationOnSpline();

	// 見た目
	UpdateVisualAnimation(
		DeltaTime
	);
}