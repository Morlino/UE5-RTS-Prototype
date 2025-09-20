// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTSUnit.generated.h"

UCLASS()
class RTSPROJECT_API ARTSUnit : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARTSUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 TeamID;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetSelected(bool bSelected);
	void MoveToLocation(const FVector &TargetLocation);
	void Attack(ARTSUnit *Unit);

private:
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class UCapsuleComponent *CapsuleComponent;

	UPROPERTY(EditAnywhere, Category = "Unit Mesh")
	class UStaticMeshComponent *UnitMesh;

	UPROPERTY(VisibleAnywhere, Category = "Selection")
	class UDecalComponent *SelectionDecal;

	UPROPERTY(VisibleAnywhere, Category = "MovementComponent")
	class UFloatingPawnMovement *MovementComponent;

	class AAIController *AIController;

	float Health = 400.0f;
	float MaxHealth = 400.0f;
	float MovementSpeed = 100.0f;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	virtual void Die();
};
