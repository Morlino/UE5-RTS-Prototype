// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "Blueprint/UserWidget.h"
#include "RTSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

ARTSPlayerController::ARTSPlayerController()
{
    // Optional: initialize variables here
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
}

// Called every frame
void ARTSPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsLMouseHolding)
    {
        GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y);
        if (ARTSHUD *RTSHUD = Cast<ARTSHUD>(GetHUD()))
        {
            RTSHUD->UpdateSelection(CurrentMousePos);
        }
    }
}

void ARTSPlayerController::SetTeamID(int32 TeamID)
{
    MyTeamID = TeamID;
}

bool ARTSPlayerController::IsLMouseHolding() const
{
    return bIsLMouseHolding;
}

void ARTSPlayerController::BeginPlay()
{
    Super::BeginPlay();

    bShowMouseCursor = true;
    FInputModeGameAndUI InputMode;
    InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways);
    InputMode.SetHideCursorDuringCapture(false);
    SetInputMode(InputMode);

    CameraPawn = Cast<ARTSCameraPawn>(GetPawn());
}

void ARTSPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Camera Inputs
    InputComponent->BindAxis("CameraMoveForward", this, &ARTSPlayerController::CameraMoveForward);
    InputComponent->BindAxis("CameraMoveRight", this, &ARTSPlayerController::CameraMoveRight);
    InputComponent->BindAxis("CameraZoom", this, &ARTSPlayerController::CameraZoom);
    InputComponent->BindAction("CameraDrag", IE_Pressed, this, &ARTSPlayerController::CameraOnDragStart);
    InputComponent->BindAction("CameraDrag", IE_Released, this, &ARTSPlayerController::CameraOnDragEnd);
    InputComponent->BindAxis("CameraDragX", this, &ARTSPlayerController::CameraDragX);
    InputComponent->BindAxis("CameraDragY", this, &ARTSPlayerController::CameraDragY);

    // Unit Control
    InputComponent->BindAction("RightClick", IE_Pressed, this, &ARTSPlayerController::OnRMouseDown);
    InputComponent->BindAction("LeftClick", IE_Pressed, this, &ARTSPlayerController::OnLMouseDown);
    InputComponent->BindAction("LeftClick", IE_Released, this, &ARTSPlayerController::OnLMouseUp);
}

void ARTSPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
}

void ARTSPlayerController::CameraMoveForward(float Value)
{
    if (CameraPawn)
    {
        CameraPawn->MoveForward(Value);
    }
}

void ARTSPlayerController::CameraMoveRight(float Value)
{
    if (CameraPawn)
    {
        CameraPawn->MoveRight(Value);
    }
}

void ARTSPlayerController::CameraZoom(float Value)
{
    if (CameraPawn)
    {
        CameraPawn->Zoom(Value);
    }
}

void ARTSPlayerController::CameraOnDragStart()
{
    if (CameraPawn)
    {
        CameraPawn->OnDragStart();
    }
}

void ARTSPlayerController::CameraOnDragEnd()
{
    if (CameraPawn)
    {
        CameraPawn->OnDragEnd();
    }
}

void ARTSPlayerController::CameraDragX(float Value)
{
    if (CameraPawn)
    {
        CameraPawn->DragX(Value);
    }
}

void ARTSPlayerController::CameraDragY(float Value)
{
    if (CameraPawn)
    {
        CameraPawn->DragY(Value);
    }
}

void ARTSPlayerController::OnLMouseDown()
{
    bIsLMouseHolding = true;
    GetMousePosition(InitialMousePos.X, InitialMousePos.Y);
    if (ARTSHUD *RTSHUD = Cast<ARTSHUD>(GetHUD()))
    {
        RTSHUD->StartSelection(InitialMousePos);
    }
}

void ARTSPlayerController::OnLMouseUp()
{
    bIsLMouseHolding = false;
    if (ARTSHUD *RTSHUD = Cast<ARTSHUD>(GetHUD()))
    {
        RTSHUD->EndSelection();
    }

    UpdateUnitSelection();
}

void ARTSPlayerController::OnRMouseDown()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRMouseDown"));

    FHitResult HitResult;
    if (!GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
        return;

    DrawDebugSphere(GetWorld(), HitResult.Location, 50.0f, 12, FColor::Green, false, 0.05f, 0, 2.0f);

    ARTSUnit *HitUnit = Cast<ARTSUnit>(HitResult.GetActor());
    if (HitUnit)
    {
        if (HitUnit->TeamID != MyTeamID)
        {
            // Attack enemy
            IssueCommandToUnits(SelectedUnits, EUnitCommand::Attack, FVector::ZeroVector, HitUnit);
        }
        else
        {
            // Follow ally
            IssueCommandToUnits(SelectedUnits, EUnitCommand::Follow, FVector::ZeroVector, HitUnit);
        }
    }
    else
    {
        // Move to ground
        IssueCommandToUnits(SelectedUnits, EUnitCommand::Move, HitResult.Location, nullptr);
    }
}

void ARTSPlayerController::UpdateUnitSelection()
{
    // Define The Selection Rectangle
    float MinX = FMath::Min(InitialMousePos.X, CurrentMousePos.X);
    float MaxX = FMath::Max(InitialMousePos.X, CurrentMousePos.X);
    float MinY = FMath::Min(InitialMousePos.Y, CurrentMousePos.Y);
    float MaxY = FMath::Max(InitialMousePos.Y, CurrentMousePos.Y);

    TArray<ARTSUnit *> UnitsInRectangle;

    for (TActorIterator<ARTSUnit> It(GetWorld()); It; ++It)
    {
        ARTSUnit *Unit = *It;
        if (IsUnitOverlappingSelectionRect(Unit, FVector2D(MinX, MinY), FVector2D(MaxX, MaxY)))
        {
            if (Unit->TeamID == MyTeamID)
            {
                UnitsInRectangle.Add(Unit);
            }
        }
    }

    // Update Selection
    if (UnitsInRectangle.Num() > 0)
    {
        ClearSelection();
        for (ARTSUnit *Unit : UnitsInRectangle)
        {
            UE_LOG(LogTemp, Warning, TEXT("Added new Unit"));
            AddUnitToSelection(Unit);
        }
    }
}

bool ARTSPlayerController::IsUnitOverlappingSelectionRect(ARTSUnit *Unit, const FVector2D &Min, const FVector2D &Max) const
{
    // Get unit's bounding box in world space
    FBox Bounds = Unit->GetComponentsBoundingBox();

    FVector2D UnitMin(TNumericLimits<float>::Max(), TNumericLimits<float>::Max());
    FVector2D UnitMax(TNumericLimits<float>::Lowest(), TNumericLimits<float>::Lowest());

    // Project all 8 corners of the bounding box
    FVector Corners[8] = {
        FVector(Bounds.Min.X, Bounds.Min.Y, Bounds.Min.Z),
        FVector(Bounds.Max.X, Bounds.Min.Y, Bounds.Min.Z),
        FVector(Bounds.Min.X, Bounds.Max.Y, Bounds.Min.Z),
        FVector(Bounds.Max.X, Bounds.Max.Y, Bounds.Min.Z),
        FVector(Bounds.Min.X, Bounds.Min.Y, Bounds.Max.Z),
        FVector(Bounds.Max.X, Bounds.Min.Y, Bounds.Max.Z),
        FVector(Bounds.Min.X, Bounds.Max.Y, Bounds.Max.Z),
        FVector(Bounds.Max.X, Bounds.Max.Y, Bounds.Max.Z)};

    for (FVector Corner : Corners)
    {
        FVector2D ScreenPos;
        if (UGameplayStatics::ProjectWorldToScreen(this, Corner, ScreenPos))
        {
            UnitMin.X = FMath::Min(UnitMin.X, ScreenPos.X);
            UnitMin.Y = FMath::Min(UnitMin.Y, ScreenPos.Y);
            UnitMax.X = FMath::Max(UnitMax.X, ScreenPos.X);
            UnitMax.Y = FMath::Max(UnitMax.Y, ScreenPos.Y);
        }
    }

    // Check for overlap
    return !(Max.X < UnitMin.X || Min.X > UnitMax.X ||
             Max.Y < UnitMin.Y || Min.Y > UnitMax.Y);
}

void ARTSPlayerController::AddUnitToSelection(ARTSUnit *Unit)
{
    SelectedUnits.Add(Unit);
    Unit->SetSelected(true);
}

void ARTSPlayerController::RemoveUnitFromSelection(ARTSUnit *Unit)
{
    SelectedUnits.Remove(Unit);
    Unit->SetSelected(false);
}

void ARTSPlayerController::ClearSelection()
{
    for (ARTSUnit *Unit : SelectedUnits)
    {
        if (Unit)
        {
            Unit->SetSelected(false);
        }
    }
    SelectedUnits.Empty();
}

void ARTSPlayerController::IssueCommandToUnits(const TArray<ARTSUnit *> &Units, EUnitCommand Command, const FVector &TargetLocation, ARTSUnit *TargetUnit)
{
    if (Units.Num() == 0) return;

    // Only compute formation offsets for Move commands
    TArray<FVector> MoveDestinations;
    if (Command == EUnitCommand::Move)
    {
        MoveDestinations = ComputeUnitDestinations(TargetLocation, Units.Num(), 100.f); // 100.f spacing
    }

    for (int32 i = 0; i < Units.Num(); i++)
    {
        ARTSUnit *Unit = Units[i];
        if (!Unit)
            continue;

        switch (Command)
        {
        case EUnitCommand::Move:
            // Each unit gets its own offset destination
            Unit->MoveTo(MoveDestinations[i]);
            break;

        case EUnitCommand::Attack:
            if (TargetUnit)
                Unit->StartAttack(TargetUnit);
            break;

        case EUnitCommand::Follow:
            if (TargetUnit)
                Unit->Follow(TargetUnit);
            break;

        default:
            break;
        }
    }
}

TArray<FVector> ARTSPlayerController::ComputeUnitDestinations(const FVector &Center, int32 NumUnits, float Spacing)
{
    TArray<FVector> Destinations;

    if (NumUnits == 0)
        return Destinations;

    int32 Rows = FMath::CeilToInt(FMath::Sqrt(static_cast<float>(NumUnits)));
    int32 Cols = FMath::CeilToInt(static_cast<float>(NumUnits) / Rows);

    for (int32 i = 0; i < NumUnits; i++)
    {
        int32 Row = i / Cols;
        int32 Col = i % Cols;

        FVector Offset((Col - Cols / 2) * Spacing, (Row - Rows / 2) * Spacing, 0.f);
        Destinations.Add(Center + Offset);
    }

    return Destinations;
}
