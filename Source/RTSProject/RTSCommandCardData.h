// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RTSCommandCardData.generated.h"

UENUM(BlueprintType)
enum class EUnitAction : uint8
{
	Move,
	Stop,
	Hold,
	Patrol,
	Attack,
	Build
};

USTRUCT(BlueprintType)
struct FCommandData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GridIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D *Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKey Hotkey;
};

UCLASS()
class RTSPROJECT_API URTSCommandCardData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "RTS")
	TMap<EUnitAction, FCommandData> Commands;

	const FCommandData *GetCommand(EUnitAction Action) const;
};
