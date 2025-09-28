#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSUnit.h"
#include "RTSCommandCardData.h"
#include "RTSPlayerController.generated.h"

UENUM(BlueprintType)
enum class EPlayerControllerState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	Selecting UMETA(DisplayName = "Selecting"),
	IssuingCommand UMETA(DisplayName = "Issuing Command"),
	PlacingStructure UMETA(DisplayName = "Placing Structure"),
};

UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// Constructor (if needed)
	ARTSPlayerController();

	// Camera Pawn reference
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ARTSCameraPawn *CameraPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<URTSCommandCardData*> CurrentCommandPage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	URTSCommandCardData *PendingCommand = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlayerControllerState CurrentControllerState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ARTSHUD *Hud;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class ARTSPlayerState *PS;

	// Selected units
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<ARTSUnit *> SelectedUnits;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ARTSUnit *CurrentSelectedUnit;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Team")

	bool IsLMouseHolding() const;

	FORCEINLINE ARTSPlayerState* GetRTSPlayerState() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Input setup
	virtual void SetupInputComponent() override;

	virtual void PostInitializeComponents() override;

	virtual void OnRep_PlayerState() override;

private:
	// Mouse selection state
	FVector2D InitialMousePos;
	FVector2D CurrentMousePos;
	bool bIsAddingToSelection = false;
	float GridSize = 200.0f;

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

	void HandleIdleRClick();
	void HandleTargetClick(AActor* Target);
	void HandleGroundClick(const FVector& Location);

	// --------------------------
	// Command Card control
	// --------------------------
	void OnCommandCard(FKey Key);
	void HandleCommandCard(URTSCommandCardData* Cmd);
	void ClearCommandCard();
	void CancelCurrentAction();
	void UpdateDisplayedCommandCard();
	URTSCommandCardData *GetCommandByType(ECommandType Type, ARTSUnit *Unit);
	URTSCommandCardData* FindCommandByHotkey(FKey Key) const;



	// --------------------------
	// Unit selection helpers
	// --------------------------
	bool UpdateUnitSelection();
	void UpdateCurrentUnit();
	bool IsUnitOverlappingSelectionRect(ARTSUnit *Unit, const FVector2D &Min, const FVector2D &Max) const;
	void BeginSelection();
	void EndSelection();
	AActor *GetActorUnderCursor();
	FVector GetLocationUnderCursor();
	FVector GetSnappedCursorLocation();
	bool IsTargetableActor(AActor *Target);
	void AddUnitToSelection(ARTSUnit *Unit);
	void RemoveUnitFromSelection(ARTSUnit *Unit);
	void ClearSelection();

	// --------------------------
	// Unit commands
	// --------------------------
	UFUNCTION(Server, Reliable)
	void ServerRequestUnitCommand(ARTSUnit* Unit, URTSCommandCardData* Cmd, FVector Location = FVector::ZeroVector, AActor* Target = nullptr);
	void AllSelectedUnitsIssueCommand(URTSCommandCardData *Cmd, FVector Location = FVector::ZeroVector, AActor *Target = nullptr);
	void IssueCurrentUnitToBuild(FVector BuildLocation);
	TArray<FVector> ComputeUnitDestinations(const FVector &Center, int32 NumUnits, float Spacing);
};
