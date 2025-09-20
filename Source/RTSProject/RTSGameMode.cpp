// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSGameMode.h"
#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "RTSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

ARTSGameMode::ARTSGameMode()
{
    PlayerControllerClass = ARTSPlayerController::StaticClass();
    DefaultPawnClass = ARTSCameraPawn::StaticClass();
    HUDClass = ARTSHUD::StaticClass();
}

void ARTSGameMode::PostLogin(APlayerController *NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UE_LOG(LogTemp, Warning, TEXT("NumPlayers=%d"), NumPlayers);

    ARTSPlayerController *RTSController = Cast<ARTSPlayerController>(NewPlayer);
    if (!RTSController)
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to cast PlayerController"));
        return;
    }

    // Assign TeamID
    AssignTeamID(RTSController, NumPlayers);

    // Choose PlayerStart based on PlayerStartTag
    FName TargetTag = (NumPlayers == 1) ? FName("Player1") : FName("Player2");
    APlayerStart *ChosenStart = FindPlayerStartByTag(TargetTag);

    // Spawn pawn and possess
    SpawnAndPossessPawn(NewPlayer, ChosenStart);
}

void ARTSGameMode::AssignTeamID(ARTSPlayerController *NewController, int32 PlayerIndex)
{
    int32 AssignedTeamID = (PlayerIndex == 1) ? 1 : 2; // Can expand later
    NewController->SetTeamID(AssignedTeamID);
    UE_LOG(LogTemp, Warning, TEXT("Assigned TeamID %d to player"), AssignedTeamID);
}

APlayerStart *ARTSGameMode::FindPlayerStartByTag(FName TargetTag)
{
    TArray<AActor *> PlayerStarts;
    UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);

    if (PlayerStarts.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("No PlayerStarts found!"));
        return nullptr;
    }

    for (AActor *Actor : PlayerStarts)
    {
        APlayerStart *PS = Cast<APlayerStart>(Actor);
        if (PS && PS->PlayerStartTag == TargetTag)
        {
            return PS;
        }
    }

    UE_LOG(LogTemp, Warning, TEXT("No PlayerStart found with tag %s"), *TargetTag.ToString());
    return nullptr;
}

void ARTSGameMode::SpawnAndPossessPawn(APlayerController *NewPlayer, APlayerStart *ChosenStart)
{
    if (!ChosenStart || !DefaultPawnClass)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot spawn pawn: Missing PlayerStart or DefaultPawnClass"));
        return;
    }

    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = NewPlayer;

    APawn *Pawn = GetWorld()->SpawnActor<APawn>(
        DefaultPawnClass,
        ChosenStart->GetActorLocation(),
        ChosenStart->GetActorRotation(),
        SpawnParams);

    if (Pawn)
    {
        NewPlayer->Possess(Pawn);
        UE_LOG(LogTemp, Warning, TEXT("Player possessed pawn at %s"), *ChosenStart->GetName());
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn pawn"));
    }
}
