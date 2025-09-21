// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "RTSPlayerController.h"

// Sets default values
ARTSCameraPawn::ARTSCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = DesiredArmLength;
	SpringArm->SetRelativeRotation(CameraRotation);
	SpringArm->bDoCollisionTest = false;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void ARTSCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<ARTSPlayerController>(GetController());

}

// Called every frame
void ARTSCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PC && !PC->GetLocalPlayer())
		return;

	// Lock Camera if Player hold Left Click
	if (PC->IsLMouseHolding())
		return;

	FVector TotalVelocity = GetTotalVelocity(); // WASD + edge scroll

	if (bIsDragging)
	{
		FVector DragVelocity = (-GetActorRightVector() * DragInput.X + -GetActorForwardVector() * DragInput.Y) * DragCameraSpeed;
		TotalVelocity += DragVelocity;
	}

	SetActorLocation(GetActorLocation() + TotalVelocity * DeltaTime);

	SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, DesiredArmLength, DeltaTime, ZoomInterpSpeed);

	float Alpha = (SpringArm->TargetArmLength - MinZoom) / (MaxZoom - MinZoom);
	float NewPitch = FMath::Lerp(MaxPitch, MinPitch, Alpha);
	SpringArm->SetRelativeRotation(FRotator(NewPitch, SpringArm->GetRelativeRotation().Yaw, 0.0f));
}

void ARTSCameraPawn::MoveForward(float Value)
{
	InputVelocity.X = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ARTSCameraPawn::MoveRight(float Value)
{
	InputVelocity.Y = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ARTSCameraPawn::Zoom(float Value)
{
	DesiredArmLength = FMath::Clamp(DesiredArmLength - Value * ZoomSpeed, MinZoom, MaxZoom);
}

FVector ARTSCameraPawn::GetTotalVelocity()
{
	return GetKeyboardVelocity() + GetEdgeScrollVelocity();
}

FVector ARTSCameraPawn::GetKeyboardVelocity()
{
	return (GetActorForwardVector() * InputVelocity.X + GetActorRightVector() * InputVelocity.Y) * InputCameraSpeed;
}

FVector ARTSCameraPawn::GetEdgeScrollVelocity()
{
	// Reset velocities each frame
	FVector EdgeScrollVelocity = FVector::ZeroVector;

	// Get Cursor location
	int32 ViewportSizeX, ViewportSizeY;
	float MouseX, MouseY;
	PC->GetViewportSize(ViewportSizeX, ViewportSizeY);
	PC->GetMousePosition(MouseX, MouseY);

	bool bViewportFocused = PC->GetLocalPlayer()->ViewportClient->Viewport->HasFocus();
	UE_LOG(LogTemp, Warning, TEXT("bViewportFocused=%d"), bViewportFocused);

	if (MouseY < BorderOffset)
		EdgeScrollVelocity += GetActorForwardVector();
	else if (MouseY > ViewportSizeY - BorderOffset)
		EdgeScrollVelocity += -GetActorForwardVector();

	if (MouseX < BorderOffset)
		EdgeScrollVelocity += -GetActorRightVector();
	else if (MouseX > ViewportSizeX - BorderOffset)
		EdgeScrollVelocity += GetActorRightVector();

	return EdgeScrollVelocity * EdgeScrollCameraSpeed;
}

void ARTSCameraPawn::OnDragStart()
{
	bIsDragging = true;
}

void ARTSCameraPawn::OnDragEnd()
{
	bIsDragging = false;
	DragInput = FVector2D::ZeroVector;
}

void ARTSCameraPawn::DragX(float Value)
{
	if (bIsDragging)
	{
		DragInput.X = Value;
	}
}

void ARTSCameraPawn::DragY(float Value)
{
	if (bIsDragging)
	{
		DragInput.Y = Value;
	}
}