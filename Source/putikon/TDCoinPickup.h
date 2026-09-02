#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TDCoinPickup.generated.h"

class UStaticMeshComponent;

UCLASS()
class PUTIKON_API ATDCoinPickup : public AActor
{
	GENERATED_BODY()

public:
	ATDCoinPickup();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Coin")
	UStaticMeshComponent* CoinMesh;

	// このコイン1枚で増える金額
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	int32 MoneyValue = 1;

	// プレイヤーに吸われ始める距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float AttractionRadius = 300.0f;

	// 取得判定距離
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float CollectRadius = 60.0f;

	// 吸引速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float AttractionSpeed = 700.0f;

	// 最初に上へ飛ぶ強さ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float InitialUpSpeed = 350.0f;

	// 横方向に飛び散る強さ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float InitialHorizontalSpeed = 300.0f;

	// 落下加速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float Gravity = 980.0f;

	// 地面から少し浮かせる
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float GroundOffset = 15.0f;

	// 回転速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Coin")
	float RotationSpeed = 120.0f;

private:
	FVector MoveVelocity;

	bool bLanded = false;

	float GroundZ = 0.0f;

	void FindGround();

	void UpdateDropMovement(float DeltaTime);

	void UpdateAttraction(float DeltaTime);

	void CollectCoin();
};