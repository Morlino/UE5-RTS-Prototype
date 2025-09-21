// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RTSHUD.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSHUD : public AHUD
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UResourceWidget> ResourceWidgetClass;

	UPROPERTY(meta = (BindWidget))
	class UResourceWidget *ResourceWidget;
	virtual void DrawHUD() override;

	void StartSelection(const FVector2D& StartPos);
	void UpdateSelection(const FVector2D &CurrentPos);
	void EndSelection();

	class UResourceWidget *GetResourceWidget() const;
	void UpdateResourceDisplay(int32 Metal, int32 Fuel, int32 CurrentSupply, int32 MaxSupply);

protected :
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	bool bIsSelecting = false;
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};
