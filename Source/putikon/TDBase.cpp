#include "TDBase.h"
#include "Components/StaticMeshComponent.h"

ATDBase::ATDBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BaseMesh =
		CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));

	RootComponent = BaseMesh;
}

void ATDBase::BeginPlay()
{
	Super::BeginPlay();

}

void ATDBase::EnemyReachedGoal()
{
	if (RemainingChildren <= 0)
	{
		return;
	}

	RemainingChildren--;

	OnChildrenChanged(RemainingChildren);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Remaining Children: %d"),
		RemainingChildren
	);

	if (RemainingChildren <= 0)
	{
		OnGameOver();
	}
}