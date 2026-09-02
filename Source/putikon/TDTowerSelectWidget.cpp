#include "TDTowerSelectWidget.h"

#include "TDBuildTree.h"
#include "TDTowerBuildData.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Kismet/GameplayStatics.h"

void UTDTowerSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FindWidgetReferences();
}

void UTDTowerSelectWidget::FindWidgetReferences()
{
	TextTowerName =
		Cast<UTextBlock>(
			GetWidgetFromName(
				TEXT("Text_TowerName")
			)
		);

	TextCost =
		Cast<UTextBlock>(
			GetWidgetFromName(
				TEXT("Text_Cost")
			)
		);

	TowerIcon =
		Cast<UImage>(
			GetWidgetFromName(
				TEXT("Image_TowerIcon")
			)
		);

	if (!TextTowerName)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Text_TowerName was not found.")
		);
	}

	if (!TextCost)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Text_Cost was not found.")
		);
	}
}

void UTDTowerSelectWidget::OpenForTree(
	ATDBuildTree* InTree
)
{
	if (!IsValid(InTree))
	{
		return;
	}

	if (InTree->GetTowerOptionCount() <= 0)
	{
		return;
	}

	CurrentTree = InTree;

	SelectedIndex = 0;

	// ”O‚Ì‚½‚ßWidgetŽQÆ‚ðÄŽæ“¾
	if (!TextTowerName || !TextCost)
	{
		FindWidgetReferences();
	}

	SetPlayerMovementEnabled(
		false
	);

	SetVisibility(
		ESlateVisibility::Visible
	);

	RefreshDisplay();
}

void UTDTowerSelectWidget::SelectNext()
{
	if (!IsValid(CurrentTree))
	{
		return;
	}

	const int32 Count =
		CurrentTree->GetTowerOptionCount();

	if (Count <= 0)
	{
		return;
	}

	SelectedIndex++;

	if (SelectedIndex >= Count)
	{
		SelectedIndex = 0;
	}

	RefreshDisplay();
}

void UTDTowerSelectWidget::SelectPrevious()
{
	if (!IsValid(CurrentTree))
	{
		return;
	}

	const int32 Count =
		CurrentTree->GetTowerOptionCount();

	if (Count <= 0)
	{
		return;
	}

	SelectedIndex--;

	if (SelectedIndex < 0)
	{
		SelectedIndex =
			Count - 1;
	}

	RefreshDisplay();
}

bool UTDTowerSelectWidget::ConfirmSelection()
{
	if (!IsValid(CurrentTree))
	{
		return false;
	}

	const bool bBuilt =
		CurrentTree->BuildTowerByIndex(
			SelectedIndex
		);

	// ‚¨‹à•s‘«‚È‚Ç‚ÅŒšÝ‚Å‚«‚È‚©‚Á‚½ê‡
	if (!bBuilt)
	{
		return false;
	}

	CurrentTree = nullptr;

	SetVisibility(
		ESlateVisibility::Collapsed
	);

	SetPlayerMovementEnabled(
		true
	);

	return true;
}

void UTDTowerSelectWidget::CancelSelection()
{
	if (!IsValid(CurrentTree))
	{
		return;
	}

	CurrentTree = nullptr;

	SetVisibility(
		ESlateVisibility::Collapsed
	);

	SetPlayerMovementEnabled(
		true
	);
}

void UTDTowerSelectWidget::RefreshDisplay()
{
	if (!IsValid(CurrentTree))
	{
		return;
	}

	UTDTowerBuildData* TowerData =
		CurrentTree->GetTowerOption(
			SelectedIndex
		);

	if (!IsValid(TowerData))
	{
		return;
	}

	if (TextTowerName)
	{
		TextTowerName->SetText(
			TowerData->DisplayName
		);
	}

	if (TextCost)
	{
		TextCost->SetText(
			FText::AsNumber(
				TowerData->Cost
			)
		);
	}

	if (
		TowerIcon &&
		TowerData->Icon
		)
	{
		TowerIcon->SetBrushFromTexture(
			TowerData->Icon
		);
	}
}

void UTDTowerSelectWidget::SetPlayerMovementEnabled(
	bool bEnabled
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

	UCharacterMovementComponent* Movement =
		Player->GetCharacterMovement();

	if (!Movement)
	{
		return;
	}

	if (bEnabled)
	{
		Movement->SetMovementMode(
			MOVE_Walking
		);
	}
	else
	{
		Movement->DisableMovement();
	}
}