// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "Blueprint/UserWidget.h"
#include "RTSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
#include "ResourceWidget.h"
#include "Net/UnrealNetwork.h"
#include "RTSPlayerState.h"

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
        if (RTSHUD)
        {
            RTSHUD->UpdateSelection(CurrentMousePos);
        }
    }

    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);
    if (bIsPlacingBuilding && Hit.bBlockingHit)
    {
        FVector SnappedLocation = Hit.Location;
        SnappedLocation.X = FMath::GridSnap(SnappedLocation.X, GridSize);
        SnappedLocation.Y = FMath::GridSnap(SnappedLocation.Y, GridSize);
        RTSHUD->UpdateBuildingGhostLocation(SnappedLocation);
    }
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
    PS = GetPlayerState<ARTSPlayerState>();
    
    // Set HUD
    RTSHUD = Cast<ARTSHUD>(GetHUD());
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

    // Command Card
    InputComponent->BindAction("CommandCardQ", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardW", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardE", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardR", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardT", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);

    InputComponent->BindAction("CommandCardA", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardS", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardD", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardF", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardG", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);

    InputComponent->BindAction("CommandCardZ", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardX", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardC", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
    InputComponent->BindAction("CommandCardV", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);

    // Always Cancel
    InputComponent->BindAction("CommandCardB", IE_Pressed, this, &ARTSPlayerController::OnCommandCard);
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
    switch (CurrentControllerState)
    {
    case EPlayerControllerState::Idle:
        BeginSelection();
        break;

    case EPlayerControllerState::Selecting:
        UE_LOG(LogTemp, Warning, TEXT("Same State"));
        break;

    case EPlayerControllerState::IssuingCommand:
        if (PendingCommand->bIsActorTargeted)
        {
            AActor *ActorUnderCursor = GetActorUnderCursor();
            if (ActorUnderCursor)
            {
                AllSelectedUnitsIssueCommand(PendingCommand, FVector::ZeroVector, ActorUnderCursor);
            }
        }
        else if (PendingCommand->bIsLocationTargeted)
        {
            FVector LocationUnderCursor = GetLocationUnderCursor();
            if (!LocationUnderCursor.IsZero())
            {
                AllSelectedUnitsIssueCommand(PendingCommand, LocationUnderCursor, nullptr);
            }
        }
        break;

    case EPlayerControllerState::PlacingStructure:
    {
        FVector SnappedCursorLocation = GetSnappedCursorLocation();
        if (!SnappedCursorLocation.IsZero())
        {
            IssueCurrentUnitToBuild(SnappedCursorLocation);
        }
        break;
    }

    default:
        break;
    }
}

void ARTSPlayerController::OnLMouseUp()
{
    if (CurrentControllerState == EPlayerControllerState::PlacingStructure ||
        CurrentControllerState == EPlayerControllerState::IssuingCommand)
        return;

    EndSelection();

    bool bSelectionChanged = UpdateUnitSelection();
    if (bSelectionChanged)
    {
        UpdateCurrentUnit();
        UpdateDisplayedCommandCard();
    }
}

void ARTSPlayerController::OnRMouseDown()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRMouseDown"));

    switch (CurrentControllerState)
    {
    case EPlayerControllerState::Idle:
    {
        // Think about this later
        UE_LOG(LogTemp, Warning, TEXT("Idle So moving"));
        AActor *Target = GetActorUnderCursor();
        if (Target->ActorHasTag("Unit") || Target->ActorHasTag("Resource"))
        {
            UE_LOG(LogTemp, Warning, TEXT("Hit some Target"));
        }
        else
        {
            // No Target, just Move
            FVector Location = GetLocationUnderCursor();
            UE_LOG(LogTemp, Warning, TEXT("Moving to %s"), *Location.ToString());
            ServerIssueCommand(SelectedUnits, ECommandType::Move, Location, nullptr);
        }

        // FHitResult HitResult;
        // if (!GetHitResultUnderCursor(ECC_Visibility, true, HitResult))
        // return;
        
        // DrawDebugSphere(GetWorld(), HitResult.Location, 50.0f, 12, FColor::Green, false, 0.05f, 0, 2.0f);
        
        // ARTSUnit *HitUnit = Cast<ARTSUnit>(HitResult.GetActor());
        // if (HitUnit)
        // {
        //     ARTSPlayerState *LocalPS = GetPlayerState<ARTSPlayerState>();
        //     if (LocalPS)
        //     {
        //         if (HitUnit->TeamID != LocalPS->TeamID)
        //         {
        //             // Attack enemy
        //             ServerIssueCommand(SelectedUnits, ECommandType::Attack, FVector::ZeroVector, HitUnit);
        //         }
        //         else
        //         {
        //             // Follow ally
        //             ServerIssueCommand(SelectedUnits, ECommandType::Move, FVector::ZeroVector, HitUnit);
        //         }
        //     }
        // }
        // else
        // {
        //     // Move to ground
        //     ServerIssueCommand(SelectedUnits, ECommandType::Move, HitResult.Location, nullptr);
        // }
    }

    default:
        break;
    }
}

void ARTSPlayerController::OnCommandCard(FKey Key)
{
    // Always cancel current action
    if (Key == EKeys::B)
    {
        CancelCurrentAction();
        return;
    }

    if (SelectedUnits.IsEmpty())
        return;

    UE_LOG(LogTemp, Warning, TEXT("Pressing %s"), *Key.ToString());

    TArray<URTSCommandCardData *> &SearchArray =
        CurrentCommandPage.Num() > 0 ? CurrentCommandPage : CurrentSelectedUnit->CommandCardData;

    URTSCommandCardData **CmdPtr = Algo::FindByPredicate(
        SearchArray,
        [Key](URTSCommandCardData *Card)
        { return Card && Card->Hotkey == Key; });

    if (!CmdPtr)
        return;
    URTSCommandCardData *Cmd = *CmdPtr;

    if (Cmd->SubCommands.Num() > 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Switched to Submenu"));
        CurrentCommandPage = Cmd->SubCommands;
        RTSHUD->UpdateCommandCard(CurrentCommandPage);
    }
    else if (Cmd->bIsBuilding) // new property on command
    {
        PendingCommand = Cmd;
        RTSHUD->SetBuildingPlacementCursor(Cmd->PreviewMesh); // show ghost mesh
        bIsPlacingBuilding = true;
        return;
    }
    else if (Cmd->bIsActorTargeted || Cmd->bIsLocationTargeted) // new check for targeted commands
    {
        PendingCommand = Cmd;            // wait for player to click a location/target
        RTSHUD->SetCommandCursor();
        CurrentCommandPage.Empty();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Execute command immediately"));
        for (ARTSUnit *Unit : SelectedUnits)
            if (Unit)
                ServerRequestUnitCommand(Unit, Cmd);

        CurrentCommandPage.Empty();
    }
}

void ARTSPlayerController::CancelCurrentAction()
{
    if (bIsPlacingBuilding)
    {
        RTSHUD->ClearBuildingPlacementCursor();
        PendingCommand = nullptr;
        bIsPlacingBuilding = false;
    }

    // Reset command card display
    CurrentCommandPage.Empty();

    if (!SelectedUnits.IsEmpty())
    {
        // Show the default command page of the first selected unit
        RTSHUD->UpdateCommandCard(CurrentSelectedUnit->CommandCardData);
    }

    UE_LOG(LogTemp, Warning, TEXT("Cancelled action"));
}

bool ARTSPlayerController::UpdateUnitSelection()
{
    TArray<ARTSUnit *> NewSelection;

    // Build NewSelection the same way as before
    float MinX = FMath::Min(InitialMousePos.X, CurrentMousePos.X);
    float MaxX = FMath::Max(InitialMousePos.X, CurrentMousePos.X);
    float MinY = FMath::Min(InitialMousePos.Y, CurrentMousePos.Y);
    float MaxY = FMath::Max(InitialMousePos.Y, CurrentMousePos.Y);

    for (TActorIterator<ARTSUnit> It(GetWorld()); It; ++It)
    {
        ARTSUnit *Unit = *It;
        if (IsUnitOverlappingSelectionRect(Unit, FVector2D(MinX, MinY), FVector2D(MaxX, MaxY)))
        {
            ARTSPlayerState *LocalPS = GetPlayerState<ARTSPlayerState>();
            if (LocalPS && Unit->TeamID == LocalPS->TeamID)
            {
                NewSelection.Add(Unit);
            }
        }
    }

    // Check if selection actually changed
    bool bSelectionChanged = false;

    if (NewSelection.Num() != SelectedUnits.Num())
    {
        bSelectionChanged = true;
    }
    else
    {
        for (int32 i = 0; i < NewSelection.Num(); i++)
        {
            if (NewSelection[i] != SelectedUnits[i])
            {
                bSelectionChanged = true;
                break;
            }
        }
    }

    if (bSelectionChanged)
    {
        ClearSelection();
        for (ARTSUnit *Unit : NewSelection)
        {
            AddUnitToSelection(Unit);
        }
    }

    return bSelectionChanged; // tell caller whether anything changed
}

void ARTSPlayerController::UpdateCurrentUnit()
{
    if (SelectedUnits.Num() > 0)
    {
        CurrentSelectedUnit = SelectedUnits[0];
    }
    else
    {
        CurrentSelectedUnit = nullptr;
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

void ARTSPlayerController::BeginSelection()
{
    CurrentControllerState = EPlayerControllerState::Selecting;
    bIsLMouseHolding = true;
    GetMousePosition(InitialMousePos.X, InitialMousePos.Y);
    if (RTSHUD)
    {
        RTSHUD->SetDefaultCursor();
        RTSHUD->StartSelection(InitialMousePos);
    }
}

void ARTSPlayerController::EndSelection()
{
    CurrentControllerState = EPlayerControllerState::Idle;
    bIsLMouseHolding = false;
    if (RTSHUD)
        RTSHUD->EndSelection();
}

void ARTSPlayerController::UpdateDisplayedCommandCard()
{
    if (SelectedUnits.Num() > 0 && RTSHUD)
    {
        RTSHUD->UpdateCommandCard(CurrentSelectedUnit->CommandCardData);
    }
}

AActor *ARTSPlayerController::GetActorUnderCursor()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (!Hit.bBlockingHit)
        return nullptr;

    return Hit.GetActor();
}

FVector ARTSPlayerController::GetLocationUnderCursor()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);

    if (!Hit.bBlockingHit)
        return FVector::ZeroVector;

    return Hit.Location;
}

FVector ARTSPlayerController::GetSnappedCursorLocation()
{
    FHitResult Hit;
    GetHitResultUnderCursor(ECC_Visibility, false, Hit);
    if (!Hit.bBlockingHit)
        return FVector::ZeroVector;
        
    DrawDebugSphere(GetWorld(), Hit.Location, 50.0f, 12, FColor::Red, false, 0.05f, 0, 2.0f);
    FVector SnappedLocation = Hit.Location;
    SnappedLocation.X = FMath::GridSnap(SnappedLocation.X, GridSize);
    SnappedLocation.Y = FMath::GridSnap(SnappedLocation.Y, GridSize);
    return SnappedLocation;
}

void ARTSPlayerController::IssueCurrentUnitToBuild(FVector BuildLocation)
{
    if (CurrentSelectedUnit->IsBusy())
        return;

    ServerRequestUnitCommand(CurrentSelectedUnit, PendingCommand, BuildLocation);

    RTSHUD->ClearBuildingPlacementCursor();

    PendingCommand = nullptr;
    bIsPlacingBuilding = false;
    CurrentControllerState = EPlayerControllerState::Idle;
}

void ARTSPlayerController::AllSelectedUnitsIssueCommand(URTSCommandCardData *Cmd, FVector Location, AActor *Target)
{
    for (ARTSUnit *Unit : SelectedUnits)
    {
        if (Unit)
        {
            ServerRequestUnitCommand(Unit, Cmd, Location, Target);
        }
    }
    CurrentControllerState = EPlayerControllerState::Idle;
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

void ARTSPlayerController::ServerIssueCommand_Implementation(const TArray<ARTSUnit *> &Units, ECommandType Command, const FVector &TargetLocation, ARTSUnit *TargetUnit)
{
    if (Units.Num() == 0)
        return;

    // Only compute formation offsets for Move commands
    TArray<FVector> MoveDestinations;
    if (Command == ECommandType::Move)
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
            case ECommandType::Move:
                if (TargetUnit) // Move command with a unit target
                {
                    if (TargetUnit->TeamID != Unit->TeamID)
                        Unit->StartAttack(TargetUnit); // enemy
                    else
                        Unit->Follow(TargetUnit); // ally
                }
                else // Move command with a location
                {
                    Unit->Move(MoveDestinations[i]);
                }
                break;

            case ECommandType::Attack:
                if (TargetUnit)
                    Unit->StartAttack(TargetUnit);
                break;
        }
    }
}

void ARTSPlayerController::ServerRequestUnitCommand_Implementation(ARTSUnit *Unit, URTSCommandCardData *Cmd, FVector Location, AActor *Target)
{
    if (Unit)
    {
        Unit->ServerExecuteCommand(Cmd, Location, Target);
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
