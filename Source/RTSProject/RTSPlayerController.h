#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSUnit.h"
#include "RTSPlayerController.generated.h"

UENUM(BlueprintType)
enum class EUnitCommand : uint8
{
	Move UMETA(DisplayName = "Move"),
	Attack UMETA(DisplayName = "Attack"),
	Follow UMETA(DisplayName = "Follow")
};

UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor (if needed)
	ARTSPlayerController();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Team info
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 MyTeamID;

	UFUNCTION(BlueprintCallable, Category = "Team")
	void SetTeamID(int32 TeamID);

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

	// Selected units
	UPROPERTY()
	TArray<ARTSUnit *> SelectedUnits;

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
	void IssueCommandToUnits(const TArray<ARTSUnit *> &Units, EUnitCommand Command, const FVector &TargetLocation = FVector::ZeroVector, ARTSUnit *TargetUnit = nullptr);
	void MoveSelectedUnitsTo(const TArray<FVector> &Destinations);
	TArray<FVector> ComputeUnitDestinations(const FVector &Center, int32 NumUnits, float Spacing);
};
