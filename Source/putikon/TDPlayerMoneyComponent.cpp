#include "TDPlayerMoneyComponent.h"

UTDPlayerMoneyComponent::UTDPlayerMoneyComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UTDPlayerMoneyComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentMoney = StartingMoney;

	OnMoneyChanged.Broadcast(
		CurrentMoney
	);
}

bool UTDPlayerMoneyComponent::SpendMoney(
	int32 Amount
)
{
	if (Amount <= 0)
	{
		return true;
	}

	if (CurrentMoney < Amount)
	{
		return false;
	}

	CurrentMoney -= Amount;

	OnMoneyChanged.Broadcast(
		CurrentMoney
	);

	return true;
}

void UTDPlayerMoneyComponent::AddMoney(
	int32 Amount
)
{
	if (Amount <= 0)
	{
		return;
	}

	CurrentMoney += Amount;

	OnMoneyChanged.Broadcast(
		CurrentMoney
	);
}