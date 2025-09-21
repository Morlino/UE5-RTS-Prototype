// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RTSPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	// Team info
	UPROPERTY(BlueprintReadWrite, Replicated, EditAnywhere, Category = "Team")
	int32 TeamID;

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
	int32 Metal;

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
	int32 Fuel;

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
	int32 CurrentSupply;

	UPROPERTY(ReplicatedUsing = OnRep_Resources)
	int32 MaxSupply;

	UFUNCTION()
	void OnRep_Resources();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
};
