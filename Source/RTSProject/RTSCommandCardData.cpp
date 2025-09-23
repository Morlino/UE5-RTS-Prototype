// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCommandCardData.h"

const FCommandData *URTSCommandCardData::GetCommand(ECommandType Action) const
{
    return Commands.Find(Action);
}