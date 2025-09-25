// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSWorker.h"
#include "RTSTownhall.h"
#include "AIController.h"
#include "Net/UnrealNetwork.h"

void ARTSWorker::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority() && bIsMovingToBuild)
    {
        float Distance = FVector::Dist(GetActorLocation(), BuildTargetLocation);
        if (Distance <= 300.0f)
        {
            // Build
            GetWorld()->SpawnActor<AActor>(CurrentCommand->BuildingClass, BuildTargetLocation, FRotator::ZeroRotator);
            bIsMovingToBuild = false;
            CurrentCommand = nullptr;
        }
    }
}

void ARTSWorker::ServerDoUniqueCommand(URTSCommandCardData *Cmd, FVector Location, AActor *Target)
{
    if (Cmd->CommandType == ECommandType::BuildTownhall)
    {
        CurrentCommand = Cmd;
        bIsMovingToBuild = true;
        BuildTargetLocation = Location;
        Move(Location);
    }
}

void ARTSWorker::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARTSWorker, bIsMovingToBuild);
    DOREPLIFETIME(ARTSWorker, BuildTargetLocation);
    DOREPLIFETIME(ARTSWorker, CurrentCommand);
}
