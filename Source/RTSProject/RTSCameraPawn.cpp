// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"

// Sets default values
ARTSCameraPawn::ARTSCameraPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// Create SpringArm
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = CameraHeight;
	SpringArm->SetRelativeRotation(CameraRotation);
	SpringArm->bDoCollisionTest = false;

	// Create Camera
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	// Enable pawn to receive input
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void ARTSCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	PC = Cast<APlayerController>(GetController());

}

// Called every frame
void ARTSCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!PC && !PC->GetLocalPlayer())
		return;

	FVector NewLocation = GetActorLocation() + (GetTotalVelocity() * DeltaTime);
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void ARTSCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ARTSCameraPawn::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARTSCameraPawn::MoveRight);
}

void ARTSCameraPawn::MoveForward(float Value)
{
	InputVelocity.X = FMath::Clamp(Value, -1.0f, 1.0f);
}

void ARTSCameraPawn::MoveRight(float Value)
{
	InputVelocity.Y = FMath::Clamp(Value, -1.0f, 1.0f);
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
