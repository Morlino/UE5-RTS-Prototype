// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSUnit.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;

public :
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MyTeamID;

	bool IsLMouseHolding();
	void SetSelected(bool bSelected);
	void SetTeamID(int32 TeamID);

private:
	class ARTSCameraPawn *CameraPawn;
	// class ARTSHUD *RTSHUD;

	// Click Control
	bool bIsLMouseHolding = false;
	FVector2D InitialMousePos;
	FVector2D CurrentMousePos;

	// Unit Manager
	UPROPERTY()
	TArray<ARTSUnit*> SelectedUnits;
	
	bool bIsAddingToSelection = false;

	// Input / Movement Functions
	void CameraMoveForward(float Value);
	void CameraMoveRight(float Value);
	void CameraZoom(float Value);
	void CameraOnDragStart();
	void CameraOnDragEnd();
	void CameraDragX(float Value);
	void CameraDragY(float Value);

	// Mouse Click Control
	void OnRMouseDown();
	void OnLMouseDown();
	void OnLMouseUp();

	TArray<FVector> ComputeUnitDestinations(const FVector &TargetLocation);
	void MoveSelectedUnitsTo(const TArray<FVector>& Destinations);

	// Unit Selection
	void UpdateUnitSelection();
	bool IsUnitOverlappingSelectionRect(ARTSUnit *Unit, const FVector2D &Min, const FVector2D &Max);
	void AddUnitToSelection(ARTSUnit *Unit);
	void RemoveUnitFromSelection(ARTSUnit *Unit);
	void ClearSelection();
};
