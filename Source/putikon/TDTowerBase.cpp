#include "TDTowerBase.h"
#include "TDEnemy.h"
#include "TDProjectile.h"
#include "TDCannonProjectile.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

#include "TimerManager.h"
#include "DrawDebugHelpers.h"

ATDTowerBase::ATDTowerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	Root =
		CreateDefaultSubobject<USceneComponent>(
			TEXT("Root")
		);

	SetRootComponent(Root);

	// -------------------------
	// タワー回転用Root
	// -------------------------

	TowerRotationRoot =
		CreateDefaultSubobject<USceneComponent>(
			TEXT("TowerRotationRoot")
		);

	TowerRotationRoot->SetupAttachment(
		Root
	);

	// -------------------------
	// 攻撃射程
	// -------------------------

	AttackRange =
		CreateDefaultSubobject<USphereComponent>(
			TEXT("AttackRange")
		);

	AttackRange->SetupAttachment(
		Root
	);

	AttackRange->SetCollisionEnabled(
		ECollisionEnabled::QueryOnly
	);

	AttackRange->SetCollisionResponseToAllChannels(
		ECR_Ignore
	);

	AttackRange->SetCollisionResponseToChannel(
		ECC_WorldDynamic,
		ECR_Overlap
	);

	// プレイヤーには影響しない
	AttackRange->SetCollisionResponseToChannel(
		ECC_Pawn,
		ECR_Ignore
	);
}

void ATDTowerBase::BeginPlay()
{
	Super::BeginPlay();

	// -------------------------
	// 攻撃射程
	// -------------------------

	AttackRange->SetSphereRadius(
		Range
	);

	// -------------------------
	// BP側のTowerMesh取得
	// -------------------------

	TowerMeshComponent =
		FindComponentByClass<UStaticMeshComponent>();

	if (TowerMeshComponent &&
		TowerMeshComponent->GetStaticMesh())
	{
		// -------------------------
		// タワーをすり抜け可能にする
		// -------------------------

		TowerMeshComponent->SetCollisionEnabled(
			ECollisionEnabled::NoCollision
		);

		// -------------------------
		// メッシュを地面に合わせる
		// -------------------------

		const FBoxSphereBounds Bounds =
			TowerMeshComponent
			->GetStaticMesh()
			->GetBounds();

		const float BottomZ =
			Bounds.Origin.Z
			- Bounds.BoxExtent.Z;

		const float ScaleZ =
			TowerMeshComponent
			->GetRelativeScale3D()
			.Z;

		FVector MeshLocation =
			TowerMeshComponent
			->GetRelativeLocation();

		MeshLocation.Z =
			-(BottomZ * ScaleZ);

		TowerMeshComponent->SetRelativeLocation(
			MeshLocation
		);

		// -------------------------
		// 回転軸をメッシュ中央へ
		// -------------------------

		const FVector MeshCenter =
			TowerMeshComponent
			->Bounds
			.Origin;

		FVector RotationRootLocation =
			GetActorLocation();

		RotationRootLocation.X =
			MeshCenter.X;

		RotationRootLocation.Y =
			MeshCenter.Y;

		TowerRotationRoot->SetWorldLocation(
			RotationRootLocation
		);

		// -------------------------
		// Meshを回転Rootの子にする
		// -------------------------

		TowerMeshComponent->AttachToComponent(
			TowerRotationRoot,
			FAttachmentTransformRules
			::KeepWorldTransform
		);
	}

	// -------------------------
	// 攻撃タイマー
	// -------------------------

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATDTowerBase::Attack,
		AttackInterval,
		true
	);
}

void ATDTowerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// -------------------------
	// 常に最優先の敵を探す
	// -------------------------

	FindTarget();

	// -------------------------
	// 常にその敵を向く
	// -------------------------

	RotateTowardTarget();

	// -------------------------
	// タワーに触れている間だけ
	// 射程表示
	// -------------------------

	if (IsPlayerTouchingTower())
	{
		FVector CircleLocation =
			GetActorLocation();

		CircleLocation.Z += 5.0f;

		DrawDebugCircle(
			GetWorld(),
			CircleLocation,
			Range,
			96,
			FColor::Green,
			false,
			0.0f,
			0,
			4.0f,
			FVector(1.0f, 0.0f, 0.0f),
			FVector(0.0f, 1.0f, 0.0f),
			false
		);
	}
}

void ATDTowerBase::FindTarget()
{
	TArray<AActor*> OverlappingActors;

	AttackRange->GetOverlappingActors(
		OverlappingActors,
		ATDEnemy::StaticClass()
	);

	CurrentTarget = nullptr;

	float FurthestDistance =
		-TNumericLimits<float>::Max();

	for (AActor* Actor : OverlappingActors)
	{
		ATDEnemy* Enemy =
			Cast<ATDEnemy>(Actor);

		if (!IsValid(Enemy))
		{
			continue;
		}

		const float EnemyDistance =
			Enemy->GetDistanceAlongSpline();

		// 一番ゴールに近い敵
		if (EnemyDistance > FurthestDistance)
		{
			FurthestDistance =
				EnemyDistance;

			CurrentTarget =
				Enemy;
		}
	}
}

void ATDTowerBase::RotateTowardTarget()
{
	if (!IsValid(CurrentTarget))
	{
		return;
	}

	if (!TowerRotationRoot)
	{
		return;
	}

	FVector Direction =
		CurrentTarget->GetActorLocation()
		- TowerRotationRoot
		->GetComponentLocation();

	// 水平方向のみ
	Direction.Z = 0.0f;

	if (Direction.IsNearlyZero())
	{
		return;
	}

	FRotator LookRotation =
		Direction.Rotation();

	// -------------------------
	// モデル正面のズレを補正
	// -------------------------

	LookRotation.Yaw +=
		AimYawOffset;

	TowerRotationRoot->SetWorldRotation(
		FRotator(
			0.0f,
			LookRotation.Yaw,
			0.0f
		)
	);
}

void ATDTowerBase::Attack()
{
	FindTarget();

	if (!IsValid(CurrentTarget))
	{
		return;
	}

	UWorld* World =
		GetWorld();

	if (!World)
	{
		return;
	}

	// =========================
	// 弓塔
	// =========================
	if (AttackType ==
		ETowerAttackType::Arrow)
	{
		if (!ProjectileClass)
		{
			return;
		}

		FVector SpawnLocation =
			GetActorLocation();

		SpawnLocation.Z +=
			ProjectileSpawnHeight;

		FVector Direction =
			CurrentTarget
			->GetActorLocation()
			- SpawnLocation;

		if (Direction.IsNearlyZero())
		{
			return;
		}

		Direction.Normalize();

		const FRotator SpawnRotation =
			Direction.Rotation();

		FActorSpawnParameters
			SpawnParams;

		SpawnParams
			.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod
			::AlwaysSpawn;

		ATDProjectile* Projectile =
			World->SpawnActor<
			ATDProjectile
			>(
				ProjectileClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

		if (!Projectile)
		{
			return;
		}

		Projectile
			->InitializeProjectile(
				CurrentTarget,
				AttackDamage
			);

		return;
	}

	// =========================
	// 大砲
	// =========================
	if (AttackType ==
		ETowerAttackType::Cannon)
	{
		if (!CannonProjectileClass)
		{
			return;
		}

		FVector SpawnLocation =
			GetActorLocation();

		SpawnLocation.Z +=
			ProjectileSpawnHeight;

		// ★この瞬間の敵の位置を保存
		FVector ImpactLocation =
			CurrentTarget
			->GetActorLocation();

		// 敵の足元を探す
		FHitResult HitResult;

		const FVector TraceStart =
			ImpactLocation +
			FVector(
				0.0f,
				0.0f,
				300.0f
			);

		const FVector TraceEnd =
			ImpactLocation -
			FVector(
				0.0f,
				0.0f,
				1000.0f
			);

		FCollisionQueryParams
			QueryParams;

		QueryParams.AddIgnoredActor(
			this
		);

		QueryParams.AddIgnoredActor(
			CurrentTarget
		);

		const bool bHit =
			World
			->LineTraceSingleByChannel(
				HitResult,
				TraceStart,
				TraceEnd,
				ECC_Visibility,
				QueryParams
			);

		if (bHit)
		{
			ImpactLocation =
				HitResult.Location;
		}

		FVector Direction =
			ImpactLocation -
			SpawnLocation;

		const FRotator SpawnRotation =
			Direction.Rotation();

		FActorSpawnParameters
			SpawnParams;

		SpawnParams
			.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod
			::AlwaysSpawn;

		ATDCannonProjectile*
			CannonProjectile =
			World->SpawnActor<
			ATDCannonProjectile
			>(
				CannonProjectileClass,
				SpawnLocation,
				SpawnRotation,
				SpawnParams
			);

		if (!CannonProjectile)
		{
			return;
		}

		CannonProjectile
			->InitializeProjectile(
				ImpactLocation,
				AttackDamage,
				CannonProjectileSpeed,
				CannonArcHeight,
				CannonExplosionRadius,
				CannonMinDamagePercent,
				CannonShockwaveDuration
			);
	}
}

bool ATDTowerBase::IsPlayerTouchingTower() const
{
	if (!TowerMeshComponent)
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

	// -------------------------
	// タワーメッシュの見た目の範囲
	// -------------------------

	const FBox TowerBox =
		TowerMeshComponent
		->Bounds
		.GetBox();

	const FVector PlayerLocation =
		Player->GetActorLocation();

	// プレイヤー位置と
	// タワーBoundsとの最短距離
	const float DistanceSquared =
		TowerBox
		.ComputeSquaredDistanceToPoint(
			PlayerLocation
		);

	return DistanceSquared <=
		FMath::Square(TouchMargin);
}