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

	SetRootComponent(ProjectileMesh);

	// ìñÇΩÇËîªíËÇÕégÇÌÇ»Ç¢
	ProjectileMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	// UEïWèÄCube
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(
		TEXT("/Engine/BasicShapes/Cube.Cube")
	);

	if (CubeMesh.Succeeded())
	{
		ProjectileMesh->SetStaticMesh(
			CubeMesh.Object
		);

		// ç◊í∑Ç¢ñ_Ç…Ç∑ÇÈ
		ProjectileMesh->SetRelativeScale3D(
			FVector(
				1.6f,
				0.6f,
				0.6f
			)
		);
	}
}

void ATDProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ATDProjectile::InitializeProjectile(
	ATDEnemy* TargetEnemy,
	float DamageAmount
)
{
	Target = TargetEnemy;
	Damage = DamageAmount;
}

void ATDProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
		TargetLocation - CurrentLocation;

	const float Distance =
		ToTarget.Size();

	const float MoveDistance =
		MoveSpeed * DeltaTime;

	// ìGÇ‹Ç≈ìÕÇ¢ÇΩÇÁñΩíÜ
	if (Distance <= MoveDistance + HitDistance)
	{
		Target->ReceiveArrowDamage(
			Damage
		);

		Destroy();
		return;
	}

	const FVector Direction =
		ToTarget.GetSafeNormal();

	// ñ_Çêiçsï˚å¸Ç÷å¸ÇØÇÈ
	SetActorRotation(
		Direction.Rotation()
	);

	const FVector NewLocation =
		CurrentLocation
		+ Direction * MoveDistance;

	SetActorLocation(
		NewLocation
	);
}