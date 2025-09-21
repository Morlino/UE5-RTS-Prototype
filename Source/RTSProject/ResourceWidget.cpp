// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceWidget.h"
#include "Components/TextBlock.h"

void UResourceWidget::UpdateResources(int32 Metal, int32 Fuel, int32 CurrentSupply, int32 MaxSupply)
{
    if (MetalText)
        MetalText->SetText(FText::AsNumber(Metal));
    if (FuelText)
        FuelText->SetText(FText::AsNumber(Fuel));
    if (SupplyText)
        SupplyText->SetText(FText::Format(FText::FromString("{0}/{1}"), FText::AsNumber(CurrentSupply), FText::AsNumber(MaxSupply)));
}