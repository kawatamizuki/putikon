#include "TDProjectile.h"
#include "TDEnemy.h"

#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ATDProjectile::ATDProjectile()
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

	// UE標準Cubeを細長くして矢として使用
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		CubeMesh(
			TEXT("/Engine/BasicShapes/Cube.Cube")
		);

	if (CubeMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(
			CubeMesh.Object
		);

		ProjectileMesh->SetRelativeScale3D(
			FVector(
				0.8f,
				0.08f,
				0.08f
			)
		);
	}
}

void ATDProjectile::BeginPlay()
{
	Super::BeginPlay();

	// BP側に古いScaleが残っていても、
	// 実行時に必ず矢の細さへ戻す
	if (ProjectileMesh)
	{
		ProjectileMesh->SetRelativeScale3D(
			FVector(
				0.8f,
				0.08f,
				0.08f
			)
		);
	}
}

void ATDProjectile::InitializeProjectile(
	ATDEnemy* TargetEnemy,
	float DamageAmount
)
{
	Target =
		TargetEnemy;

	Damage =
		DamageAmount;
}

void ATDProjectile::Tick(
	float DeltaTime
)
{
	Super::Tick(
		DeltaTime
	);

	if (!IsValid(Target))
	{
		Destroy();
		return;
	}

	const FVector CurrentLocation =
		GetActorLocation();

	const FVector TargetLocation =
		Target->GetActorLocation();

	const FVector ToTarget =
		TargetLocation -
		CurrentLocation;

	const float Distance =
		ToTarget.Size();

	const float MoveDistance =
		MoveSpeed *
		DeltaTime;

	// 敵まで届いたら命中
	if (
		Distance <=
		MoveDistance + HitDistance
		)
	{
		Target->ReceiveArrowDamage(
			Damage
		);

		// 命中音用イベント
		OnArrowHit();

		Destroy();
		return;
	}

	const FVector Direction =
		ToTarget.GetSafeNormal();

	// 矢を進行方向へ向ける
	SetActorRotation(
		Direction.Rotation()
	);

	const FVector NewLocation =
		CurrentLocation +
		Direction *
		MoveDistance;

	SetActorLocation(
		NewLocation
	);
}