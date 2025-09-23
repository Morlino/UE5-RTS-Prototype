// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RTSCommandCardData.generated.h"

UENUM(BlueprintType)
enum class ECommandType : uint8
{
	Move UMETA(DisplayName = "Move"),
	Stop UMETA(DisplayName = "Stop"),
	Hold UMETA(DisplayName = "Hold"),
	Patrol UMETA(DisplayName = "Patrol"),
	Attack UMETA(DisplayName = "Attack"),
	Build UMETA(DisplayName = "Build"),
	BuildTownhall UMETA(DisplayName = "BuildTownhall")
};

UCLASS()
class RTSPROJECT_API URTSCommandCardData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName CommandName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECommandType CommandType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 GridIndex;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D *Icon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FKey Hotkey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<URTSCommandCardData*> SubCommands;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsTargeted = false;
};
