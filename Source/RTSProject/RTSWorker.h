// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RTSUnit.h"
#include "RTSWorker.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSWorker : public ARTSUnit
{
	GENERATED_BODY()

private:
	UPROPERTY(Replicated)
	bool bIsMovingToBuild = false;

	UPROPERTY(Replicated)
	FVector BuildTargetLocation;

	UPROPERTY(Replicated)
	URTSCommandCardData *CurrentCommand;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void ServerDoUniqueCommand(URTSCommandCardData *Cmd, FVector Location = FVector::ZeroVector, AActor *Target = nullptr) override;
};
