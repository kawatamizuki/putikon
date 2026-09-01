#include "TDBase.h"
#include "Kismet/GameplayStatics.h"
#include "TDEnemy.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"


ATDEnemy::ATDEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	EnemyMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EnemyMesh"));

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
			PathActor->FindComponentByClass<USplineComponent>();

		if (PathSpline)
		{
			const FVector StartLocation =
				PathSpline->GetLocationAtDistanceAlongSpline(
					0.0f,
					ESplineCoordinateSpace::World
				);

			SetActorLocation(StartLocation);
		}
	}
}

void ATDEnemy::ReceiveArrowDamage(float DamageAmount)
{
	const float FinalDamage =
		DamageAmount * ArrowDamageMultiplier;

	CurrentHealth -= FinalDamage;

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
}

void ATDEnemy::ReceiveCannonDamage(float DamageAmount)
{
	const float FinalDamage =
		DamageAmount * CannonDamageMultiplier;

	CurrentHealth -= FinalDamage;

	if (CurrentHealth <= 0.0f)
	{
		Destroy();
	}
}

void ATDEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PathSpline)
	{
		return;
	}

	DistanceAlongSpline += MoveSpeed * DeltaTime;

	const float SplineLength = PathSpline->GetSplineLength();

	if (DistanceAlongSpline >= SplineLength)
	{
		AActor* BaseActor = UGameplayStatics::GetActorOfClass(
			GetWorld(),
			ATDBase::StaticClass()
		);

		ATDBase* Base = Cast<ATDBase>(BaseActor);

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

	const FRotator NewRotation =
		PathSpline->GetRotationAtDistanceAlongSpline(
			DistanceAlongSpline,
			ESplineCoordinateSpace::World
		);

	SetActorLocationAndRotation(NewLocation, NewRotation);
}