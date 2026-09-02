#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TDTowerSelectWidget.generated.h"

class UTextBlock;
class UImage;
class ATDBuildTree;
class UTDTowerBuildData;

UCLASS()
class PUTIKON_API UTDTowerSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

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
	UImage* TowerIcon = nullptr;

	UPROPERTY()
	ATDBuildTree* CurrentTree = nullptr;

	int32 SelectedIndex = 0;

	void RefreshDisplay();

	void SetPlayerMovementEnabled(bool bEnabled);

	void FindWidgetReferences();
};