// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API UResourceWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Resources")
	void UpdateResources(int32 Metal, int32 Fuel, int32 CurrentSupply, int32 MaxSupply);

protected:
	// Bind these to your text blocks in the widget blueprint
	UPROPERTY(meta = (BindWidget))
	class UTextBlock *MetalText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock *FuelText;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock *SupplyText;
};
