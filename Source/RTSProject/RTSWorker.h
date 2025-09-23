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

public:
	ARTSWorker();

	virtual void DoUniqueCommand(URTSCommandCardData *Cmd) override;
};
