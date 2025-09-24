// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSHUD.h"
#include "Engine/Canvas.h"
#include "ResourceWidget.h"
#include "RTSPlayerState.h"

void ARTSHUD::BeginPlay()
{
    Super::BeginPlay();

    if (ResourceWidgetClass)
    {
        ResourceWidget = CreateWidget<UResourceWidget>(GetWorld(), ResourceWidgetClass);
        if (ResourceWidget)
        {
            ResourceWidget->AddToViewport();

            // Initialize resource display from PlayerState
            if (APlayerController *PC = GetOwningPlayerController())
            {
                if (ARTSPlayerState *PS = PC->GetPlayerState<ARTSPlayerState>())
                {
                    UpdateResourceDisplay(PS->Metal, PS->Fuel, PS->CurrentSupply, PS->MaxSupply);
                }
            }
        }
    }

    if (CommandCardWidgetClass)
    {
        CommandCardWidget = CreateWidget<UCommandCardWidget>(GetWorld(), CommandCardWidgetClass);
        if (CommandCardWidget)
        {
            CommandCardWidget->AddToViewport();
        }
    }
}

void ARTSHUD::DrawHUD()
{
    Super::DrawHUD();

    if (bIsSelecting)
    {
        FVector2D TopLeft(FMath::Min(SelectionStart.X, SelectionEnd.X), FMath::Min(SelectionStart.Y, SelectionEnd.Y));
        FVector2D BottomRight(FMath::Max(SelectionStart.X, SelectionEnd.X), FMath::Max(SelectionStart.Y, SelectionEnd.Y));

        FLinearColor FillColor = FLinearColor(0.f, 1.f, 0.f, 0.2f);  // Green with 20% opacity
        FLinearColor BorderColor = FLinearColor(0.f, 1.f, 0.f, 1.f); // Fully opaque border
        float BorderThickness = 2.0f;

        // Fill
        DrawRect(FillColor, TopLeft.X, TopLeft.Y, BottomRight.X - TopLeft.X, BottomRight.Y - TopLeft.Y);

        // Border
        DrawLine(TopLeft.X, TopLeft.Y, BottomRight.X, TopLeft.Y, BorderColor, BorderThickness);         // Top
        DrawLine(TopLeft.X, BottomRight.Y, BottomRight.X, BottomRight.Y, BorderColor, BorderThickness); // Bottom
        DrawLine(TopLeft.X, TopLeft.Y, TopLeft.X, BottomRight.Y, BorderColor, BorderThickness);         // Left
        DrawLine(BottomRight.X, TopLeft.Y, BottomRight.X, BottomRight.Y, BorderColor, BorderThickness); // Right
    }
}

void ARTSHUD::StartSelection(const FVector2D &StartPos)
{
    bIsSelecting = true;
    SelectionStart = StartPos;
    SelectionEnd = StartPos;
}

void ARTSHUD::UpdateSelection(const FVector2D &CurrentPos)
{
    SelectionEnd = CurrentPos;
}

void ARTSHUD::EndSelection()
{
    bIsSelecting = false;
}

void ARTSHUD::UpdateResourceDisplay(int32 Metal, int32 Fuel, int32 CurrentSupply, int32 MaxSupply)
{
    if (ResourceWidget)
    {
        ResourceWidget->UpdateResources(Metal, Fuel, CurrentSupply, MaxSupply);
    }
}

void ARTSHUD::UpdateCommandCard(const TArray<URTSCommandCardData *> &Commands)
{
    if (CommandCardWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("PopulateCommandCard"));
        CommandCardWidget->PopulateCommandCard(Commands);
    }
}

void ARTSHUD::SetBuildingPlacementCursor(UStaticMesh *PreviewMesh)
{
    UE_LOG(LogTemp, Warning, TEXT("SetBuildingPlacementCursor"));
}

void ARTSHUD::UpdateBuildingGhostLocation(const FVector &Location)
{
    UE_LOG(LogTemp, Warning, TEXT("UpdateBuildingGhostLocation"));
}

void ARTSHUD::ClearBuildingPlacementCursor()
{
    UE_LOG(LogTemp, Warning, TEXT("ClearBuildingPlacementCursor"));
}

void ARTSHUD::SetCommandCursor()
{
    APlayerController *PC = GetOwningPlayerController();
    if (PC)
        PC->CurrentMouseCursor = EMouseCursor::Crosshairs; // SC2-style target
}

void ARTSHUD::SetDefaultCursor()
{
    APlayerController *PC = GetOwningPlayerController();
    if (PC)
        PC->CurrentMouseCursor = EMouseCursor::Default; // standard arrow
}