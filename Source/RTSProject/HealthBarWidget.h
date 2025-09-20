// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "HealthBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	UProgressBar *HealthProgressBar;

	UFUNCTION(BlueprintCallable)
	void UpdateHealth(float Percent);
};
