// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RTSCommandCardData.h"

#include "CommandCardWidget.generated.h"

UCLASS()
class RTSPROJECT_API UCommandCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Bind to your grid panel in the BP
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel *GridPanel;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CommandCard")
	int32 NumRows = 3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CommandCard")
	int32 NumColumns = 5;

	void PopulateCommandCard(const TArray<FCommandData> &Commands);
};
