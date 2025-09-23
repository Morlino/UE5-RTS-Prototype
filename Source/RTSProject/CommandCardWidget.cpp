// Fill out your copyright notice in the Description page of Project Settings.


#include "CommandCardWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"

void UCommandCardWidget::PopulateCommandCard(const TArray<URTSCommandCardData *> &Commands)
{
    if (!GridPanel)
    {
        UE_LOG(LogTemp, Warning, TEXT("GridPanel is nullptr!"));
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Populating %d commands"), Commands.Num());

    const int32 TotalButtons = GridPanel->GetChildrenCount();

    for (int32 i = 0; i < TotalButtons; ++i)
    {
        UButton *Btn = Cast<UButton>(GridPanel->GetChildAt(i));
        if (!Btn)
            continue;

        if (i < Commands.Num() && Commands[i])
        {
            URTSCommandCardData *Cmd = Commands[i];

            if (UImage *Img = Cast<UImage>(Btn->GetChildAt(0)))
            {
                if (Cmd->Icon)
                {
                    FSlateBrush Brush = Img->GetBrush();
                    Brush.SetResourceObject(Cmd->Icon);
                    Img->SetBrush(Brush);
                }
            }

            Btn->SetVisibility(ESlateVisibility::Visible);
            Btn->SetColorAndOpacity(FLinearColor(1, 1, 1, 1));

            if (UUniformGridSlot *GridSlot = Cast<UUniformGridSlot>(Btn->Slot))
            {
                int32 Row = Cmd->GridIndex / NumColumns;
                int32 Col = Cmd->GridIndex % NumColumns;

                GridSlot->SetRow(Row);
                GridSlot->SetColumn(Col);
            }
        }
        else
        {
            Btn->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}
