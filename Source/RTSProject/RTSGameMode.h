// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RTSGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSGameMode : public AGameMode
{
	GENERATED_BODY()

	ARTSGameMode();

protected:
	// Assign a TeamID to the new player
	void AssignTeamID(class ARTSPlayerController *NewController, int32 PlayerIndex);

	// Find a PlayerStart actor by its PlayerStartTag
	APlayerStart *FindPlayerStartByTag(FName TargetTag);

	// Spawn and possess the default pawn at a given PlayerStart
	void SpawnAndPossessPawn(APlayerController *NewPlayer, APlayerStart *ChosenStart);

public:
	virtual void PostLogin(APlayerController *NewPlayer) override;
};
