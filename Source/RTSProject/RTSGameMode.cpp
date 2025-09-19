// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSGameMode.h"
#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "RTSHUD.h"

ARTSGameMode::ARTSGameMode()
{
    PlayerControllerClass = ARTSPlayerController::StaticClass();
    DefaultPawnClass = ARTSCameraPawn::StaticClass();
    HUDClass = ARTSHUD::StaticClass();
}