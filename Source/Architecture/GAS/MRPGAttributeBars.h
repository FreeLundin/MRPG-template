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
 * Designers own the visuals: reparent a UMG widget blueprint onto this class
 * and give its three progress bars the names HealthBar / StaminaBar / ManaBar
 * (matched by BindWidget). All attribute reading and live updates are handled
 * here against the owning pawn's MRPG ability system, so the Blueprint graph
 * needs no GAS wiring. This keeps the fragile (and, in the MCP authoring path,
 * error-prone) attribute plumbing out of Blueprint while designers stay in
 * control of the look and the exposed public members below.
 */
UCLASS(Abstract, Blueprintable, Category = "MRPG|HUD")
class ARCHITECTURE_API UMRPGAttributeBars : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

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
