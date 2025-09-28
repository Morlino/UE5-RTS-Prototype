// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "RTSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"
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

    switch (CurrentControllerState)
    {
    case EPlayerControllerState::Selecting:
    {
        GetMousePosition(CurrentMousePos.X, CurrentMousePos.Y);
        if (Hud)
        {
            Hud->UpdateSelection(CurrentMousePos);
        }
        break;
    }
    case EPlayerControllerState::PlacingStructure:
    {
        FHitResult Hit;
        GetHitResultUnderCursor(ECC_Visibility, false, Hit);
        if (Hit.bBlockingHit)
        {
            FVector SnappedLocation = Hit.Location;
            SnappedLocation.X = FMath::GridSnap(SnappedLocation.X, GridSize);
            SnappedLocation.Y = FMath::GridSnap(SnappedLocation.Y, GridSize);
            Hud->UpdateBuildingGhostLocation(SnappedLocation);
        }
    }

    default:
        break;
    }
}

ARTSPlayerState* ARTSPlayerController::GetRTSPlayerState() const
{
    return PS;
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
    Hud = Cast<ARTSHUD>(GetHUD());
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

void ARTSPlayerController::OnRep_PlayerState()
{
    Super::OnRep_PlayerState();

    // Update our cached pointer when replication delivers PlayerState
    PS = Cast<ARTSPlayerState>(PlayerState);

    // (Optional) You can fire off some init code here if you need
    if (PS)
    {
        UE_LOG(LogTemp, Log, TEXT("PlayerState replicated, TeamID: %d"), PS->TeamID);
    }
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
        UE_LOG(LogTemp, Warning, TEXT("Display New Selection"));
        UpdateCurrentUnit();
        UpdateDisplayedCommandCard();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Display Same Selection"));
    }
}

void ARTSPlayerController::OnRMouseDown()
{
    UE_LOG(LogTemp, Warning, TEXT("OnRMouseDown"));

    switch (CurrentControllerState)
    {
    case EPlayerControllerState::Idle:
        HandleIdleRClick();
        break;

    default:
        break;
    }
}

void ARTSPlayerController::OnCommandCard(FKey Key)
{
    if (Key == EKeys::B)
    {
        CancelCurrentAction();
        return;
    }

    if (SelectedUnits.IsEmpty())
        return;

    URTSCommandCardData* Cmd = FindCommandByHotkey(Key);
    if (!Cmd)
        return;

    HandleCommandCard(Cmd);
}

void ARTSPlayerController::HandleCommandCard(URTSCommandCardData* Cmd)
{
    if (!Cmd) return;

    if (Cmd->SubCommands.Num() > 0)
    {
        CurrentCommandPage = Cmd->SubCommands;
        Hud->UpdateCommandCard(CurrentCommandPage);
        return;
    }

    if (Cmd->bIsBuilding)
    {
        PendingCommand = Cmd;
        Hud->SetBuildingPlacementCursor(Cmd->PreviewMesh);
        ClearCommandCard();
        CurrentControllerState = EPlayerControllerState::PlacingStructure;
        return;
    }

    if (Cmd->bIsActorTargeted || Cmd->bIsLocationTargeted)
    {
        PendingCommand = Cmd;
        Hud->SetCommandCursor();
        ClearCommandCard();
        return;
    }

    // Execute immediately
    AllSelectedUnitsIssueCommand(Cmd);
    ClearCommandCard();
}

void ARTSPlayerController::ClearCommandCard()
{
    if (SelectedUnits.IsEmpty())
    {
        // Reset command card display
        CurrentCommandPage.Empty();
        Hud->UpdateCommandCard(CurrentCommandPage);
    }
    else if (!SelectedUnits.IsEmpty())
    {
        // Show the default command page of the first selected unit
        CurrentCommandPage = CurrentSelectedUnit->CommandCardData;
        Hud->UpdateCommandCard(CurrentCommandPage);
    }
}

void ARTSPlayerController::CancelCurrentAction()
{
    if (CurrentControllerState == EPlayerControllerState::PlacingStructure)
    {
        Hud->ClearBuildingPlacementCursor();
        PendingCommand = nullptr;
        CurrentControllerState = EPlayerControllerState::Idle;
    }

    ClearCommandCard();

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

    UE_LOG(LogTemp, Log, TEXT("NewSelection.Num(): %d"), NewSelection.Num());

    // If nothing is selected, don't treat it as a selection change
    if (NewSelection.Num() == 0)
    {
        return false; // Keep current selection
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
    GetMousePosition(InitialMousePos.X, InitialMousePos.Y);
    if (Hud)
    {
        Hud->SetDefaultCursor();
        Hud->StartSelection(InitialMousePos);
    }
}

void ARTSPlayerController::EndSelection()
{
    CurrentControllerState = EPlayerControllerState::Idle;
    if (Hud)
        Hud->EndSelection();
}

void ARTSPlayerController::UpdateDisplayedCommandCard()
{
    if (SelectedUnits.Num() > 0 && Hud)
    {
        Hud->UpdateCommandCard(CurrentSelectedUnit->CommandCardData);
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

    Hud->ClearBuildingPlacementCursor();

    PendingCommand = nullptr;
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

bool ARTSPlayerController::IsTargetableActor(AActor *Target)
{
    if (Target->ActorHasTag("Unit") ||
        Target->ActorHasTag("Resource") ||
        Target->ActorHasTag("Building"))
        return true;
    return false;
}

URTSCommandCardData* ARTSPlayerController::GetCommandByType(ECommandType Type, ARTSUnit* Unit)
{
    if (!Unit) return nullptr;

    for (URTSCommandCardData* Cmd : Unit->CommandCardData)
    {
        if (Cmd && Cmd->CommandType == Type)
        {
            return Cmd;
        }
    }

    return nullptr;
}

URTSCommandCardData* ARTSPlayerController::FindCommandByHotkey(FKey Key) const
{
    const TArray<URTSCommandCardData*>& SearchArray = 
        CurrentCommandPage.Num() > 0 ? CurrentCommandPage : CurrentSelectedUnit->CommandCardData;

    if (const auto CmdPtr = Algo::FindByPredicate(SearchArray,
        [Key](const URTSCommandCardData* Card) { return Card && Card->Hotkey == Key; }))
    {
        return *CmdPtr;
    }
    return nullptr;
}


void ARTSPlayerController::HandleIdleRClick()
{
    FHitResult Hit;
    if (!GetHitResultUnderCursor(ECC_Visibility, false, Hit))
        return;

    DrawDebugSphere(GetWorld(), Hit.Location, 50.0f, 12, FColor::Green, false, 0.05f, 0, 2.0f);

    AActor* Target = Hit.GetActor();
    if (Target && IsTargetableActor(Target))
    {
        HandleTargetClick(Target);
    }
    else
    {
        HandleGroundClick(Hit.Location);
    }
}

void ARTSPlayerController::HandleTargetClick(AActor* Target)
{
    if (!Target->ActorHasTag("Unit"))
        return;

    ARTSUnit* HitUnit = Cast<ARTSUnit>(Target);
    if (!HitUnit || !PS)
        return;

    if (HitUnit->TeamID != PS->TeamID)
    {
        // Attack enemy
        URTSCommandCardData* AttackCommand = GetCommandByType(ECommandType::Attack, HitUnit);
        AllSelectedUnitsIssueCommand(AttackCommand, FVector::ZeroVector, HitUnit);
    }
    else
    {
        // Follow ally
        URTSCommandCardData* MoveCommand = GetCommandByType(ECommandType::Move, HitUnit);
        AllSelectedUnitsIssueCommand(MoveCommand, FVector::ZeroVector, HitUnit);
    }
}

void ARTSPlayerController::HandleGroundClick(const FVector& Location)
{
    // Assume you want to move to this point
    if (!SelectedUnits.Num())
        return;

    // Pick Move command from the first unit (or some canonical unit)
    URTSCommandCardData* MoveCommand = GetCommandByType(ECommandType::Move, SelectedUnits[0]);
    AllSelectedUnitsIssueCommand(MoveCommand, Location, nullptr);
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
