// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTSCameraPawn.generated.h"

UCLASS()
class RTSPROJECT_API ARTSCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ARTSCameraPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Camera Components
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class USpringArmComponent *SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	class UCameraComponent *Camera;

	UPROPERTY(VisibleAnywhere)
	class ARTSPlayerController *PC;

	// Camera Setup / Initial Values
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float DesiredArmLength = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	FRotator CameraRotation = FRotator(-60.0f, 0.0f, 0.0f);

	// Velocity vectors
	FVector CurrentVelocity;
	FVector InputVelocity;

	// Movement speeds
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float InputCameraSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float EdgeScrollCameraSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float DragCameraSpeed = 4000.0f;

	// Mouse edge scroll
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	int BorderOffset = 5;

	// Zoom Settings
	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float ZoomSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float ZoomInterpSpeed = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MinZoom = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MaxZoom = 4000.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MinPitch = -60.0f;

	UPROPERTY(EditAnywhere, Category = "Camera Movement")
	float MaxPitch = -40.0f;

	bool bIsDragging = false;
	FVector2D DragInput;

	// Helper Functions
	FVector GetTotalVelocity();
	FVector GetKeyboardVelocity();
	FVector GetEdgeScrollVelocity();

public:
	// Input / Movement Functions
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Zoom(float Value);
	void OnDragStart();
	void OnDragEnd();
	void DragX(float Value);
	void DragY(float Value);
};
