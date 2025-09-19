// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RTSPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RTSPROJECT_API ARTSPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void PostInitializeComponents() override;

	public :
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	class ARTSCameraPawn *CameraPawn;
	// class ARTSHUD *RTSHUD;

	// Click Control
	float ClickThreshold = 5.0f;
	bool bIsLMouseHolding = false;
	FVector2D InitialMousePos;
	FVector2D CurrentMousePos;

	// Input / Movement Functions
	void CameraMoveForward(float Value);
	void CameraMoveRight(float Value);
	void CameraZoom(float Value);
	void CameraOnDragStart();
	void CameraOnDragEnd();
	void CameraDragX(float Value);
	void CameraDragY(float Value);

	// Unit Input
	void OnRMouseUp();
	void OnLMouseDown();
	void OnLMouseUp();

public:
	bool IsLMouseHolding();
};
