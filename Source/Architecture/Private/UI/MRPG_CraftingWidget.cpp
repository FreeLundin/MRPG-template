#include "UI/MRPG_CraftingWidget.h"
#include "Components/MRPGInventoryComponent.h"

UMRPG_CraftingWidget::UMRPG_CraftingWidget()
	: SelectedRecipe(nullptr)
{
}

void UMRPG_CraftingWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (AvailableRecipes.Num() > 0 && !SelectedRecipe)
	{
		SelectRecipe(AvailableRecipes[0]);
	}
}

void UMRPG_CraftingWidget::SelectRecipe(UMRPGCraftingRecipeDataAsset* Recipe)
{
	SelectedRecipe = Recipe;
	bool bCanCraft = false;

	if (const UMRPGInventoryComponent* Inventory = GetOwningInventory())
	{
		bCanCraft = Inventory->CanCraftRecipe(Recipe);
	}

	OnRecipeSelected(Recipe, bCanCraft);
}

bool UMRPG_CraftingWidget::CraftSelectedRecipe()
{
	if (!SelectedRecipe)
	{
		return false;
	}

	UMRPGInventoryComponent* Inventory = GetOwningInventory();
	if (!Inventory)
	{
		return false;
	}

	const bool bSuccess = Inventory->CraftRecipe(SelectedRecipe);
	OnCraftCompleted(SelectedRecipe, bSuccess);

	// Re-evaluate recipe crafting feasibility
	SelectRecipe(SelectedRecipe);
	return bSuccess;
}

