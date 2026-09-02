#include "TDTowerBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TDEnemy.h"

ATDTowerBase::ATDTowerBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	AttackRange = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRange"));
	AttackRange->SetupAttachment(Root);

	AttackRange->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttackRange->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRange->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ATDTowerBase::BeginPlay()
{
	Super::BeginPlay();

	AttackRange->SetSphereRadius(Range);

	StartAttack();
}

void ATDTowerBase::StartAttack()
{
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
		ATDEnemy* Enemy = Cast<ATDEnemy>(Actor);

		if (Enemy)
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

	// ここは次に、TDEnemy側の既存ダメージ関数へ接続する
}