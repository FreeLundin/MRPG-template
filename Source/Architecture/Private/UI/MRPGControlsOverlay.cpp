#include "UI/MRPGControlsOverlay.h"
#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/VerticalBox.h"
#include "Components/HorizontalBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBoxSlot.h"
#include "Components/HorizontalBoxSlot.h"

void UMRPGControlsOverlay::NativeConstruct()
{
	Super::NativeConstruct();

	BuildDefaultWidgetTree();
}

void UMRPGControlsOverlay::BuildDefaultWidgetTree()
{
	if (WidgetTree->RootWidget)
	{
		return;
	}

	UBorder* RootBorder = NewObject<UBorder>(this, TEXT("RootBorder"));
	if (!RootBorder)
	{
		return;
	}

	RootBorder->SetBrushColor(FLinearColor(0.04f, 0.04f, 0.07f, 0.80f));
	RootBorder->SetPadding(FMargin(16.f, 12.f, 16.f, 12.f));

	UVerticalBox* VBox = NewObject<UVerticalBox>(RootBorder, TEXT("ControlsList"));
	if (!VBox)
	{
		return;
	}

	// Title Text
	UTextBlock* TitleText = NewObject<UTextBlock>(VBox, TEXT("Title"));
	TitleText->SetText(FText::FromString(TEXT("MRPG CONTROLS (Press H to Toggle)")));
	TitleText->SetColorAndOpacity(FSlateColor(FLinearColor(1.0f, 0.82f, 0.2f, 1.0f)));
	UVerticalBoxSlot* TitleSlot = VBox->AddChildToVerticalBox(TitleText);
	TitleSlot->SetPadding(FMargin(0.f, 0.f, 0.f, 8.f));

	// Helper lambda to add control rows
	auto AddControlRow = [this, VBox](const FString& KeyBinding, const FString& ActionDesc)
	{
		UHorizontalBox* HBox = NewObject<UHorizontalBox>(VBox);
		
		UTextBlock* KeyText = NewObject<UTextBlock>(HBox);
		KeyText->SetText(FText::FromString(KeyBinding));
		KeyText->SetColorAndOpacity(FSlateColor(FLinearColor(0.35f, 0.75f, 1.0f, 1.0f)));
		
		UTextBlock* DescText = NewObject<UTextBlock>(HBox);
		DescText->SetText(FText::FromString(FString::Printf(TEXT("  -  %s"), *ActionDesc)));
		DescText->SetColorAndOpacity(FSlateColor(FLinearColor(0.9f, 0.9f, 0.9f, 1.0f)));

		HBox->AddChildToHorizontalBox(KeyText);
		HBox->AddChildToHorizontalBox(DescText);

		UVerticalBoxSlot* RowSlot = VBox->AddChildToVerticalBox(HBox);
		RowSlot->SetPadding(FMargin(0.f, 2.f, 0.f, 2.f));
	};

	AddControlRow(TEXT("[W A S D]"), TEXT("Move (Mover2 Locomotion)"));
	AddControlRow(TEXT("[Space]"), TEXT("Jump (GAS -10 Stamina)"));
	AddControlRow(TEXT("[Shift]"), TEXT("Sprint"));
	AddControlRow(TEXT("[E]"), TEXT("Interact (SmartObjects / NPCs)"));
	AddControlRow(TEXT("[RMB]"), TEXT("Aim / Strafe Mode"));
	AddControlRow(TEXT("[C]"), TEXT("Crouch"));
	AddControlRow(TEXT("[R]"), TEXT("Trigger Ragdoll"));
	AddControlRow(TEXT("[']"), TEXT("Toggle GAS Gameplay Debugger"));
	AddControlRow(TEXT("[~]"), TEXT("Open Console (Cheats / ShowDebug)"));
	AddControlRow(TEXT("[H]"), TEXT("Toggle This Overlay"));

	RootBorder->SetContent(VBox);
	WidgetTree->RootWidget = RootBorder;
}

void UMRPGControlsOverlay::ToggleVisibilityState()
{
	if (GetVisibility() == ESlateVisibility::Visible || GetVisibility() == ESlateVisibility::SelfHitTestInvisible)
	{
		SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
}

bool UMRPGControlsOverlay::IsOverlayVisible() const
{
	return GetVisibility() == ESlateVisibility::Visible || GetVisibility() == ESlateVisibility::SelfHitTestInvisible;
}

