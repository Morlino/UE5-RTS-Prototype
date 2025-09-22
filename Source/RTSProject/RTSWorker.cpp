// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSWorker.h"

ARTSWorker::ARTSWorker()
{
    UE_LOG(LogTemp, Warning, TEXT("Start Worker constructor, SupportedActions.Num = %d"), SupportedActions.Num());

    SupportedActions = {EUnitAction::Move,
                        EUnitAction::Stop,
                        EUnitAction::Attack,
                        EUnitAction::Build};

    UE_LOG(LogTemp, Warning, TEXT("End Worker constructor, SupportedActions.Num = %d"), SupportedActions.Num());
}