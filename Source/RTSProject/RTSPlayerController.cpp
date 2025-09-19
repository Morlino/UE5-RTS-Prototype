// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSPlayerController.h"
#include "RTSCameraPawn.h"
#include "Blueprint/UserWidget.h"
#include "RTSHUD.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"

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

void ARTSPlayerController::PostInitializeComponents()
{
    Super::PostInitializeComponents();
    // RTSHUD = Cast<ARTSHUD>(GetHUD());
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
    InputComponent->BindAction("RightClick", IE_Pressed, this, &ARTSPlayerController::OnRMouseUp);
    InputComponent->BindAction("LeftClick", IE_Pressed, this, &ARTSPlayerController::OnLMouseDown);
    InputComponent->BindAction("LeftClick", IE_Released, this, &ARTSPlayerController::OnLMouseUp);
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

void ARTSPlayerController::OnRMouseUp()
{
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
        if (IsUnitInSelectionRect(Unit, FVector2D(MinX, MinY), FVector2D(MaxX, MaxY)))
        {
            UnitsInRectangle.Add(Unit);
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

bool ARTSPlayerController::IsUnitInSelectionRect(ARTSUnit *Unit, const FVector2D &Min, const FVector2D &Max)
{
    FVector2D ScreenPos;
    UGameplayStatics::ProjectWorldToScreen(this, Unit->GetActorLocation(), ScreenPos);
    return ScreenPos.X >= Min.X && ScreenPos.X <= Max.X &&
           ScreenPos.Y >= Min.Y && ScreenPos.Y <= Max.Y;
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

bool ARTSPlayerController::IsLMouseHolding()
{
    return bIsLMouseHolding;
}