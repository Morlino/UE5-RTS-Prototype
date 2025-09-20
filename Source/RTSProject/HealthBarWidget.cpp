// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

void UHealthBarWidget::UpdateHealth(float Percent)
{
    if (HealthProgressBar)
    {
        HealthProgressBar->SetPercent(Percent);
    }

    FLinearColor HealthColor = FLinearColor::LerpUsingHSV(FLinearColor::Red, FLinearColor::Green, Percent);
    HealthProgressBar->SetFillColorAndOpacity(HealthColor);
}