// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSHUD.h"
#include "Engine/Canvas.h"

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