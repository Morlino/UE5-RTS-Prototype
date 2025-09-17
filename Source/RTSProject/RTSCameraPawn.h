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

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	// Camera Component
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	// Camera Setups Movement 
	UPROPERTY(EditAnywhere, Category="Camera Movement")
	float CameraHeight = 2000.0f;

	UPROPERTY(EditAnywhere, Category="Camera Movement")
	FRotator CameraRotation = FRotator(-60.0f, 0.0f, 0.0f);

	// Camera Dynamic Movement
	FVector CurrentVelocity;

	UPROPERTY(EditAnywhere, Category="Camera Movement")
	float CameraSpeed = 1000.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
};
