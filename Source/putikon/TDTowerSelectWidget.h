#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDTowerSelectWidget.generated.h"

class UTextBlock;
class UImage;
class USizeBox;
class ATDBuildTree;
class UTDTowerBuildData;

UCLASS()
class PUTIKON_API UTDTowerSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	virtual void NativeTick(
		const FGeometry& MyGeometry,
		float InDeltaTime
	) override;

	UFUNCTION(BlueprintCallable, Category = "Tower Select")
	void OpenForTree(ATDBuildTree* InTree);

	UFUNCTION(BlueprintCallable, Category = "Tower Select")
	void SelectNext();

	UFUNCTION(BlueprintCallable, Category = "Tower Select")
	void SelectPrevious();

	UFUNCTION(BlueprintCallable, Category = "Tower Select")
	bool ConfirmSelection();

	UFUNCTION(BlueprintCallable, Category = "Tower Select")
	void CancelSelection();

	UFUNCTION(BlueprintPure, Category = "Tower Select")
	bool IsMenuOpen() const
	{
		return CurrentTree != nullptr;
	}

private:
	UPROPERTY()
	UTextBlock* TextTowerName = nullptr;

	UPROPERTY()
	UTextBlock* TextCost = nullptr;

	UPROPERTY()
	UTextBlock* TextHelp = nullptr;

	UPROPERTY()
	UImage* TowerIcon = nullptr;

	UPROPERTY()
	USizeBox* MenuBox = nullptr;

	UPROPERTY()
	ATDBuildTree* CurrentTree = nullptr;

	int32 SelectedIndex = 0;

	// ÉvÉåÉCÉÑÅ[Ç©ÇÁâÊñ è„Ç≈Ç«ÇÍÇæÇØÇ∏ÇÁÇ∑Ç©
	FVector2D MenuScreenOffset =
		FVector2D(200.0f, -120.0f);

	void RefreshDisplay();

	void SetPlayerMovementEnabled(bool bEnabled);

	void FindWidgetReferences();

	void UpdateMenuPosition();
};