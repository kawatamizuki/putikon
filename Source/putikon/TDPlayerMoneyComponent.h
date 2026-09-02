#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TDPlayerMoneyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMoneyChanged,
	int32,
	NewMoney
);

UCLASS(
	ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent)
)
class PUTIKON_API UTDPlayerMoneyComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:
	UTDPlayerMoneyComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(
		EditAnywhere,
		BlueprintReadWrite,
		Category = "Money"
	)
	int32 StartingMoney = 500;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Money"
	)
	int32 CurrentMoney = 0;

	UPROPERTY(
		BlueprintAssignable,
		Category = "Money"
	)
	FOnMoneyChanged OnMoneyChanged;

	UFUNCTION(
		BlueprintCallable,
		Category = "Money"
	)
	bool SpendMoney(int32 Amount);

	UFUNCTION(
		BlueprintCallable,
		Category = "Money"
	)
	void AddMoney(int32 Amount);

	UFUNCTION(
		BlueprintPure,
		Category = "Money"
	)
	int32 GetMoney() const
	{
		return CurrentMoney;
	}
};