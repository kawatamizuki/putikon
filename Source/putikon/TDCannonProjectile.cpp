#include "TDCannonProjectile.h"
#include "TDEnemy.h"

#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "DrawDebugHelpers.h"

ATDCannonProjectile::ATDCannonProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	ProjectileMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("ProjectileMesh")
		);

	SetRootComponent(
		ProjectileMesh
	);

	ProjectileMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// UE標準のSphereを砲弾として使用
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		SphereMesh(
			TEXT("/Engine/BasicShapes/Sphere.Sphere")
		);

	if (SphereMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(
			SphereMesh.Object
		);

		ProjectileMesh->SetRelativeScale3D(
			FVector(
				0.20f,
				0.20f,
				0.20f
			)
		);
	}
}

void ATDCannonProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ATDCannonProjectile::InitializeProjectile(
	const FVector& TargetLocation,
	float DamageAmount,
	float InMoveSpeed,
	float InArcHeight,
	float InExplosionRadius,
	float InMinDamagePercent,
	float InShockwaveDuration
)
{
	StartLocation =
		GetActorLocation();

	DestinationLocation =
		TargetLocation;

	BaseDamage =
		DamageAmount;

	MoveSpeed =
		FMath::Max(
			InMoveSpeed,
			1.0f
		);

	ArcHeight =
		InArcHeight;

	ExplosionRadius =
		FMath::Max(
			InExplosionRadius,
			1.0f
		);

	MinDamagePercent =
		FMath::Clamp(
			InMinDamagePercent,
			0.0f,
			1.0f
		);

	ShockwaveDuration =
		FMath::Max(
			InShockwaveDuration,
			0.01f
		);

	TravelDistance =
		FVector::Distance(
			StartLocation,
			DestinationLocation
		);

	TravelTime =
		TravelDistance /
		MoveSpeed;

	TravelTime =
		FMath::Max(
			TravelTime,
			0.01f
		);

	ElapsedTime = 0.0f;

	bExploded = false;
}

void ATDCannonProjectile::Tick(
	float DeltaTime
)
{
	Super::Tick(
		DeltaTime
	);

	if (bExploded)
	{
		UpdateShockwave(
			DeltaTime
		);

		return;
	}

	ElapsedTime +=
		DeltaTime;

	const float Alpha =
		FMath::Clamp(
			ElapsedTime /
			TravelTime,
			0.0f,
			1.0f
		);

	// 発射地点 → 着弾地点へ直線移動
	FVector NewLocation =
		FMath::Lerp(
			StartLocation,
			DestinationLocation,
			Alpha
		);

	// 軽い放物線
	const float ArcOffset =
		FMath::Sin(
			Alpha * PI
		) * ArcHeight;

	NewLocation.Z +=
		ArcOffset;

	SetActorLocation(
		NewLocation
	);

	if (Alpha >= 1.0f)
	{
		Explode();
	}
}

void ATDCannonProjectile::Explode()
{
	if (bExploded)
	{
		return;
	}

	bExploded = true;

	// 砲弾を消す
	if (ProjectileMesh)
	{
		ProjectileMesh->SetVisibility(
			false
		);
	}

	TArray<AActor*> EnemyActors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		ATDEnemy::StaticClass(),
		EnemyActors
	);

	const FVector ExplosionLocation =
		GetActorLocation();

	for (AActor* Actor : EnemyActors)
	{
		ATDEnemy* Enemy =
			Cast<ATDEnemy>(
				Actor
			);

		if (!IsValid(Enemy))
		{
			continue;
		}

		// 高さの差は無視して平面的な距離で判定
		FVector EnemyLocation =
			Enemy->GetActorLocation();

		FVector FlatExplosionLocation =
			ExplosionLocation;

		EnemyLocation.Z = 0.0f;
		FlatExplosionLocation.Z = 0.0f;

		const float Distance =
			FVector::Distance(
				FlatExplosionLocation,
				EnemyLocation
			);

		if (Distance > ExplosionRadius)
		{
			continue;
		}

		// 0 = 中心
		// 1 = 爆発範囲の一番外側
		const float DistanceAlpha =
			FMath::Clamp(
				Distance /
				ExplosionRadius,
				0.0f,
				1.0f
			);

		// 中心100%
		// 外側50%以上
		const float DamagePercent =
			FMath::Lerp(
				1.0f,
				MinDamagePercent,
				DistanceAlpha
			);

		const float FinalDamage =
			BaseDamage *
			DamagePercent;

		Enemy->ReceiveCannonDamage(
			FinalDamage
		);
	}

	ShockwaveElapsedTime =
		0.0f;
}

void ATDCannonProjectile::UpdateShockwave(
	float DeltaTime
)
{
	ShockwaveElapsedTime +=
		DeltaTime;

	const float Alpha =
		FMath::Clamp(
			ShockwaveElapsedTime /
			ShockwaveDuration,
			0.0f,
			1.0f
		);

	const float CurrentRadius =
		FMath::Lerp(
			10.0f,
			ExplosionRadius,
			Alpha
		);

	FVector CircleLocation =
		GetActorLocation();

	CircleLocation.Z +=
		5.0f;

	DrawDebugCircle(
		GetWorld(),
		CircleLocation,
		CurrentRadius,
		64,
		FColor::Orange,
		false,
		0.0f,
		0,
		5.0f,
		FVector(
			1.0f,
			0.0f,
			0.0f
		),
		FVector(
			0.0f,
			1.0f,
			0.0f
		),
		false
	);

	if (ShockwaveElapsedTime >=
		ShockwaveDuration)
	{
		Destroy();
	}
}