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

	SetRootComponent(
		CoinMesh
	);

	CoinMesh->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);
}

void ATDCoinPickup::BeginPlay()
{
	Super::BeginPlay();

	FindGround();

	// 横方向をランダムに決める
	FVector RandomDirection(
		FMath::FRandRange(-1.0f, 1.0f),
		FMath::FRandRange(-1.0f, 1.0f),
		0.0f
	);

	if (RandomDirection.IsNearlyZero())
	{
		RandomDirection =
			FVector(
				1.0f,
				0.0f,
				0.0f
			);
	}

	RandomDirection.Normalize();

	const float RandomHorizontalSpeed =
		FMath::FRandRange(
			InitialHorizontalSpeed * 0.65f,
			InitialHorizontalSpeed
		);

	MoveVelocity =
		RandomDirection *
		RandomHorizontalSpeed;

	// 全コインを必ず上へ飛ばす
	MoveVelocity.Z =
		FMath::FRandRange(
			InitialUpSpeed * 0.9f,
			InitialUpSpeed * 1.15f
		);

	bLanded = false;
}

void ATDCoinPickup::Tick(
	float DeltaTime
)
{
	Super::Tick(
		DeltaTime
	);

	// 回転
	AddActorLocalRotation(
		FRotator(
			0.0f,
			RotationSpeed * DeltaTime,
			0.0f
		)
	);

	// 空中
	if (!bLanded)
	{
		UpdateDropMovement(
			DeltaTime
		);

		return;
	}

	// 着地後だけ吸引
	UpdateAttraction(
		DeltaTime
	);
}

void ATDCoinPickup::FindGround()
{
	UWorld* World =
		GetWorld();

	if (!World)
	{
		GroundZ =
			GetActorLocation().Z -
			100.0f;

		return;
	}

	const FVector Start =
		GetActorLocation() +
		FVector(
			0.0f,
			0.0f,
			300.0f
		);

	const FVector End =
		GetActorLocation() -
		FVector(
			0.0f,
			0.0f,
			1500.0f
		);

	FHitResult HitResult;

	FCollisionQueryParams Params;

	// 自分自身を無視
	Params.AddIgnoredActor(
		this
	);

	// コインを落とした敵も無視
	if (AActor* OwnerActor = GetOwner())
	{
		Params.AddIgnoredActor(
			OwnerActor
		);
	}

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
			GetActorLocation().Z -
			100.0f;
	}
}

void ATDCoinPickup::UpdateDropMovement(
	float DeltaTime
)
{
	// 重力を加える
	MoveVelocity.Z -=
		Gravity *
		DeltaTime;

	FVector NewLocation =
		GetActorLocation() +
		MoveVelocity *
		DeltaTime;

	// 着地
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

	const FVector CoinLocation =
		GetActorLocation();

	FVector TargetLocation =
		Player->GetActorLocation();

	TargetLocation.Z +=
		50.0f;

	const float Distance =
		FVector::Distance(
			CoinLocation,
			TargetLocation
		);

	// 取得
	if (Distance <= CollectRadius)
	{
		CollectCoin();

		return;
	}

	// 吸引範囲外
	if (Distance > AttractionRadius)
	{
		return;
	}

	// プレイヤーへ吸引
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
