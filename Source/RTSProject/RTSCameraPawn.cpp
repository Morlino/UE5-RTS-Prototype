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

	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = SceneComponent;

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
}

// Called every frame
void ARTSCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!CurrentVelocity.IsZero())
	{
		FVector NewLocation = GetActorLocation() + (CurrentVelocity * DeltaTime);
		SetActorLocation(NewLocation);
	}

	UE_LOG(LogTemp, Warning, TEXT("Pawn Location: %s"), *GetActorLocation().ToString());
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
	CurrentVelocity.X = FMath::Clamp(Value, -1.0f, 1.0f) * CameraSpeed;
}

void ARTSCameraPawn::MoveRight(float Value)
{
	CurrentVelocity.Y = FMath::Clamp(Value, -1.0f, 1.0f) * CameraSpeed;
}
