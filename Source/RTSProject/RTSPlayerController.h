#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSUnit.h"
#include "RTSCommandCardData.h"
#include "RTSPlayerController.generated.h"

UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor (if needed)
	ARTSPlayerController();

	class ARTSHUD *RTSHUD;

	class ARTSPlayerState *PS;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Team")

	bool IsLMouseHolding() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Input setup
	virtual void SetupInputComponent() override;

	virtual void PostInitializeComponents() override;

private:
	// Camera Pawn reference
	class ARTSCameraPawn *CameraPawn;

	// Mouse selection state
	bool bIsLMouseHolding = false;
	bool bIsAddingToSelection = false;
	FVector2D InitialMousePos;
	FVector2D CurrentMousePos;
	float GridSize = 200.0f;

	TArray<URTSCommandCardData*> CurrentCommandPage;
	URTSCommandCardData *PendingCommand = nullptr;

	// Selected units
	UPROPERTY()
	TArray<ARTSUnit *> SelectedUnits;
	bool bIsPlacingBuilding = false;

	// --------------------------
	// Camera control functions
	// --------------------------
	void CameraMoveForward(float Value);
	void CameraMoveRight(float Value);
	void CameraZoom(float Value);

	void CameraOnDragStart();
	void CameraOnDragEnd();
	void CameraDragX(float Value);
	void CameraDragY(float Value);

	// --------------------------
	// Mouse click control
	// --------------------------
	void OnLMouseDown();
	void OnLMouseUp();
	void OnRMouseDown();

	// --------------------------
	// Command Card control
	// --------------------------
	void OnCommandCard(FKey Key);
	void CancelCurrentAction();

	// --------------------------
	// Unit selection helpers
	// --------------------------
	void UpdateUnitSelection();
	bool IsUnitOverlappingSelectionRect(ARTSUnit *Unit, const FVector2D &Min, const FVector2D &Max) const;

	void AddUnitToSelection(ARTSUnit *Unit);
	void RemoveUnitFromSelection(ARTSUnit *Unit);
	void ClearSelection();

	// --------------------------
	// Unit commands
	// --------------------------
	UFUNCTION(Server, Reliable)
	void ServerIssueCommand(const TArray<ARTSUnit *> &Units, ECommandType Command, const FVector &TargetLocation = FVector::ZeroVector, ARTSUnit *TargetUnit = nullptr);

	UFUNCTION(Server, Reliable)
	void ServerRequestUnitCommand(ARTSUnit* Unit, URTSCommandCardData* Cmd, FVector Location = FVector::ZeroVector, AActor* Target = nullptr);
	
	TArray<FVector> ComputeUnitDestinations(const FVector &Center, int32 NumUnits, float Spacing);
};
