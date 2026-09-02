#include "TDTowerBase.h"
#include "TDEnemy.h"
#include "Components/SphereComponent.h"
#include "TimerManager.h"

ATDTowerBase::ATDTowerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(Root);

	AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRange->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
}

void ATDTowerBase::BeginPlay()
{
	Super::BeginPlay();

	AttackRange->SetSphereRadius(Range);

	GetWorldTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&ATDTowerBase::Attack,
		AttackInterval,
		true
	);
}

void ATDTowerBase::FindTarget()
{
	TArray<AActor*> OverlappingActors;

	AttackRange->GetOverlappingActors(
		OverlappingActors,
		ATDEnemy::StaticClass()
	);

	CurrentTarget = nullptr;

	for (AActor* Actor : OverlappingActors)
	{
		if (ATDEnemy* Enemy = Cast<ATDEnemy>(Actor))
		{
			CurrentTarget = Enemy;
			break;
		}
	}
}

void ATDTowerBase::Attack()
{
	if (!IsValid(CurrentTarget))
	{
		FindTarget();
	}

	if (!IsValid(CurrentTarget))
	{
		return;
	}

	// “G‚Ì•ûŒü‚ðŒü‚­
	FVector Direction =
		CurrentTarget->GetActorLocation() - GetActorLocation();

	Direction.Z = 0.0f;

	if (!Direction.IsNearlyZero())
	{
		const FRotator LookRotation = Direction.Rotation();
		SetActorRotation(LookRotation);
	}

	// UŒ‚
	switch (AttackType)
	{
	case ETowerAttackType::Arrow:
		CurrentTarget->ReceiveArrowDamage(AttackDamage);
		break;

	case ETowerAttackType::Cannon:
		CurrentTarget->ReceiveCannonDamage(AttackDamage);
		break;
	}

	FindTarget();
}