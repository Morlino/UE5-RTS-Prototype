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
	virtual void DrawHUD() override;

	void StartSelection(const FVector2D& StartPos);
	void UpdateSelection(const FVector2D &CurrentPos);
	void EndSelection();

private:
	bool bIsSelecting = false;
	FVector2D SelectionStart;
	FVector2D SelectionEnd;
};
