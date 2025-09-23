// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSWorker.h"

ARTSWorker::ARTSWorker()
{
}

void ARTSWorker::DoUniqueCommand(URTSCommandCardData *Cmd)
{
    if (Cmd->CommandType == ECommandType::BuildTownhall)
    {
        UE_LOG(LogTemp, Warning, TEXT("BuildTownhall"));
    }
}