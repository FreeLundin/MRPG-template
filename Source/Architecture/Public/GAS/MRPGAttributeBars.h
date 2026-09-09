#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MRPGAttributeBars.generated.h"

class UMRPGAbilitySystemComponent;
class UProgressBar;
class UMRPGAttributeSet;

/**
 * Thin C++ systems-layer HUD widget that visualizes the player's GAS vitals
 * (Health / Stamina / Mana) as ProgressBars.
 *
 * This class is fully self-contained: in NativeConstruct it builds its own
 * root VerticalBox with the three progress bars (HealthBar / StaminaBar /
 * ManaBar), so the HUD appears in PIE with no Blueprint asset required. All
 * attribute reading and live updates are handled here against the owning
 * pawn's MRPG ability system, so the Blueprint graph needs no GAS wiring.
 *
 * Designers who want custom visuals can still reparent a UMG widget blueprint
 * onto this class and supply their own HealthBar / StaminaBar / ManaBar named
 * bars (matched by BindWidget) to override the code-built defaults.
 */
UCLASS(Blueprintable, Category = "MRPG|HUD")
class ARCHITECTURE_API UMRPGAttributeBars : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	/** Builds the default three-bar widget tree when created without a Blueprint template. */
	void BuildDefaultWidgetTree();

	/**
	 * Re-reads Health/Stamina/Mana (and maxes) from the owning pawn's attribute
	 * set and pushes ratios to the three bars. Public and overridable so a
	 * designer/Blueprinter can branch on it or add extra bars without C++.
	 */
	UFUNCTION(BlueprintCallable, Category = "MRPG|HUD")
	virtual void UpdateBars();

	/** The resolved project ASC (for designers). Null until the player pawn is found. */
	UFUNCTION(BlueprintCallable, Category = "MRPG|HUD")
	UMRPGAbilitySystemComponent* GetAbilitySystemComponent() const;

	/** The three vitals bars. Names must match the widget tree via BindWidget. Exposed so designers can read/repoint them. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "MRPG|HUD")
	TObjectPtr<UProgressBar> HealthBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "MRPG|HUD")
	TObjectPtr<UProgressBar> StaminaBar = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget), Category = "MRPG|HUD")
	TObjectPtr<UProgressBar> ManaBar = nullptr;

private:
	/** Locates the player pawn's ASC (and its attribute set) and binds live updates. */
	bool ResolveAbilitySystem();

	/** Dynamic handler invoked by MRPGAttributeSet::OnAttributeChanged when a GameplayEffect changes an attribute. */
	UFUNCTION()
	void HandleAttributeChanged(const FGameplayAttribute& InAttribute, float OldValue, float NewValue);

	UPROPERTY()
	TObjectPtr<UMRPGAbilitySystemComponent> AbilitySystemComponent = nullptr;
};
