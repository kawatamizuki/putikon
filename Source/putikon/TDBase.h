#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDBase.generated.h"

class UStaticMeshComponent;

UCLASS()
class PUTIKON_API ATDBase : public AActor
{
	GENERATED_BODY()

public:
	ATDBase();

	UFUNCTION(BlueprintCallable, Category = "Base")
	void EnemyReachedGoal();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Base")
	UStaticMeshComponent* BaseMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base")
	int32 RemainingChildren = 3;

	UFUNCTION(BlueprintImplementableEvent, Category = "Base")
	void OnChildrenChanged(int32 NewChildrenCount);

	UFUNCTION(BlueprintImplementableEvent, Category = "Base")
	void OnGameOver();
};
