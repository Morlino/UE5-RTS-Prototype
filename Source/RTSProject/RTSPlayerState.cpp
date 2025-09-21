// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerState.h"
#include "RTSPlayerController.h"
#include "RTSHUD.h"
#include "Net/UnrealNetwork.h"

void ARTSPlayerState::BeginPlay()
{
    Super::BeginPlay();

    Metal = 50;
    Fuel = 0;
    CurrentSupply = 12;
    MaxSupply = 15;

    UE_LOG(LogTemp, Warning, TEXT("PlayerState resources initialized"));
}

void ARTSPlayerState::OnRep_Resources()
{
    if (ARTSPlayerController *PC = Cast<ARTSPlayerController>(GetOwner()))
    {
        if (ARTSHUD *RTSHUD = Cast<ARTSHUD>(PC->GetHUD()))
        {
            RTSHUD->UpdateResourceDisplay(Metal, Fuel, CurrentSupply, MaxSupply);
        }
    }
}

void ARTSPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ARTSPlayerState, Metal);
    DOREPLIFETIME(ARTSPlayerState, Fuel);
    DOREPLIFETIME(ARTSPlayerState, CurrentSupply);
    DOREPLIFETIME(ARTSPlayerState, MaxSupply);
}