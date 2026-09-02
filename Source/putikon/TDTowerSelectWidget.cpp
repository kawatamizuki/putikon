#include "TDTowerSelectWidget.h"

#include "TDBuildTree.h"
#include "TDTowerBuildData.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/CanvasPanelSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Kismet/GameplayStatics.h"

void UTDTowerSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FindWidgetReferences();
}

void UTDTowerSelectWidget::NativeTick(
	const FGeometry& MyGeometry,
	float InDeltaTime
)
{
	Super::NativeTick(
		MyGeometry,
		InDeltaTime
	);

	// メニューが開いている間だけ
	// プレイヤーの横へ追従
	if (IsValid(CurrentTree))
	{
		UpdateMenuPosition();
	}
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

	TextHelp =
		Cast<UTextBlock>(
			GetWidgetFromName(
				TEXT("Text_Help")
			)
		);

	TowerIcon =
		Cast<UImage>(
			GetWidgetFromName(
				TEXT("Image_TowerIcon")
			)
		);

	MenuBox =
		Cast<USizeBox>(
			GetWidgetFromName(
				TEXT("SizeBox_Menu")
			)
		);

	if (!MenuBox)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("SizeBox_Menu was not found.")
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

	if (
		!TextTowerName ||
		!TextCost ||
		!TextHelp ||
		!MenuBox
		)
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

	// 開いた瞬間にも位置を更新
	UpdateMenuPosition();
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
		const FText CostText =
			FText::Format(
				FText::FromString(
					TEXT("{0}円")
				),
				FText::AsNumber(
					TowerData->Cost
				)
			);

		TextCost->SetText(
			CostText
		);
	}

	if (TextHelp)
	{
		TextHelp->SetText(
			TowerData->Description
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

void UTDTowerSelectWidget::UpdateMenuPosition()
{
	if (!MenuBox)
	{
		return;
	}

	ACharacter* Player =
		UGameplayStatics::GetPlayerCharacter(
			GetWorld(),
			0
		);

	if (!IsValid(Player))
	{
		return;
	}

	APlayerController* PlayerController =
		UGameplayStatics::GetPlayerController(
			GetWorld(),
			0
		);

	if (!IsValid(PlayerController))
	{
		return;
	}

	// プレイヤーの頭付近を基準にする
	FVector WorldLocation =
		Player->GetActorLocation();

	WorldLocation.Z +=
		100.0f;

	FVector2D ScreenPosition;

	const bool bProjected =
		UWidgetLayoutLibrary
		::ProjectWorldLocationToWidgetPosition(
			PlayerController,
			WorldLocation,
			ScreenPosition,
			false
		);

	if (!bProjected)
	{
		return;
	}

	// プレイヤーの少し右上へ
	ScreenPosition +=
		MenuScreenOffset;

	UCanvasPanelSlot* CanvasSlot =
		Cast<UCanvasPanelSlot>(
			MenuBox->Slot
		);

	if (!CanvasSlot)
	{
		return;
	}

	CanvasSlot->SetPosition(
		ScreenPosition
	);

	// 左上を基準に配置
	CanvasSlot->SetAlignment(
		FVector2D(
			0.0f,
			0.5f
		)
	);
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