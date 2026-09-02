#include "TDCoinPickup.h"

#include "TDPlayerMoneyComponent.h"

#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ATDCoinPickup::ATDCoinPickup()
{
	PrimaryActorTick.bCanEverTick = true;

	CoinMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(
			TEXT("CoinMesh")
		);

	SetRootComponent(CoinMesh);

	CoinMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);
}

void ATDCoinPickup::BeginPlay()
{
	Super::BeginPlay();

	FindGround();

	const FVector RandomDirection =
		FVector(
			FMath::FRandRange(-1.0f, 1.0f),
			FMath::FRandRange(-1.0f, 1.0f),
			0.0f
		).GetSafeNormal();

	const float RandomHorizontalSpeed =
		FMath::FRandRange(
			InitialHorizontalSpeed * 0.5f,
			InitialHorizontalSpeed
		);

	MoveVelocity =
		RandomDirection * RandomHorizontalSpeed;

	MoveVelocity.Z =
		FMath::FRandRange(
			InitialUpSpeed * 0.8f,
			InitialUpSpeed * 1.2f
		);
}

void ATDCoinPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// コインをくるくる回す
	AddActorLocalRotation(
		FRotator(
			0.0f,
			RotationSpeed * DeltaTime,
			0.0f
		)
	);

	if (!bLanded)
	{
		UpdateDropMovement(DeltaTime);
	}

	UpdateAttraction(DeltaTime);
}

void ATDCoinPickup::FindGround()
{
	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	const FVector Start =
		GetActorLocation() +
		FVector(0.0f, 0.0f, 300.0f);

	const FVector End =
		GetActorLocation() -
		FVector(0.0f, 0.0f, 1000.0f);

	FHitResult HitResult;

	FCollisionQueryParams Params;

	Params.AddIgnoredActor(this);

	const bool bHit =
		World->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECC_Visibility,
			Params
		);

	if (bHit)
	{
		GroundZ =
			HitResult.Location.Z +
			GroundOffset;
	}
	else
	{
		GroundZ =
			GetActorLocation().Z;
	}
}

void ATDCoinPickup::UpdateDropMovement(
	float DeltaTime
)
{
	MoveVelocity.Z -=
		Gravity * DeltaTime;

	FVector NewLocation =
		GetActorLocation() +
		MoveVelocity * DeltaTime;

	if (NewLocation.Z <= GroundZ)
	{
		NewLocation.Z =
			GroundZ;

		bLanded = true;

		MoveVelocity =
			FVector::ZeroVector;
	}

	SetActorLocation(
		NewLocation
	);
}

void ATDCoinPickup::UpdateAttraction(
	float DeltaTime
)
{
	ACharacter* Player =
		UGameplayStatics::GetPlayerCharacter(
			GetWorld(),
			0
		);

	if (!IsValid(Player))
	{
		return;
	}

	const FVector PlayerLocation =
		Player->GetActorLocation();

	const FVector CoinLocation =
		GetActorLocation();

	const float Distance =
		FVector::Distance(
			CoinLocation,
			PlayerLocation
		);

	if (Distance <= CollectRadius)
	{
		CollectCoin();
		return;
	}

	if (Distance > AttractionRadius)
	{
		return;
	}

	// 吸引中は落下処理を終了
	bLanded = true;

	FVector TargetLocation =
		PlayerLocation;

	// プレイヤーの腰くらいへ吸い寄せる
	TargetLocation.Z += 50.0f;

	const FVector NewLocation =
		FMath::VInterpConstantTo(
			CoinLocation,
			TargetLocation,
			DeltaTime,
			AttractionSpeed
		);

	SetActorLocation(
		NewLocation
	);
}

void ATDCoinPickup::CollectCoin()
{
	ACharacter* Player =
		UGameplayStatics::GetPlayerCharacter(
			GetWorld(),
			0
		);

	if (!IsValid(Player))
	{
		return;
	}

	UTDPlayerMoneyComponent* MoneyComponent =
		Player->FindComponentByClass<
		UTDPlayerMoneyComponent
		>();

	if (!MoneyComponent)
	{
		return;
	}

	MoneyComponent->AddMoney(
		MoneyValue
	);

	Destroy();
}
