// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "RTSCommandCardData.h"

#include "RTSUnit.generated.h"

UENUM(BlueprintType)
enum class EUnitState : uint8
{
	Idle 		UMETA(DisplayName = "Idle"),
	Moving 		UMETA(DisplayName = "Moving"),
	Attacking 	UMETA(DisplayName = "Attacking"),
	Following 	UMETA(DisplayName = "Following")
};

UCLASS()
class RTSPROJECT_API ARTSUnit : public APawn
{
	GENERATED_BODY()

public:
	// Constructor
	ARTSUnit();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Team info
	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere, Category = "Unit")
	int32 TeamID;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Command")
	TArray<URTSCommandCardData*> CommandCardData;

	// Public interface for unit behavior
	void SetSelected(bool bSelected);
	void Move(const FVector &TargetLocation);
	void Follow(const ARTSUnit *TargetUnit);
	void StartAttack(ARTSUnit *TargetUnit);
	void Attack(ARTSUnit *TargetUnit);
	void StopAttack();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Health = 400.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 400.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MovementSpeed = 1000.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackDamage = 50.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackSpeed = 1.f;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float AttackRange = 100.f;

	// Unit state
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EUnitState CurrentState = EUnitState::Idle;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	FVector MoveTarget;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	ARTSUnit *CurrentTarget = nullptr;

	float TimeSinceLastAttack = 0.0f;
	bool bIsMovingToTarget = false;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Internal update methods (can be overridden in derived classes)
	virtual void UpdateMovement(float DeltaTime);
	virtual void UpdateAttack(float DeltaTime);
	virtual void UpdateFollow(float DeltaTime);

private:
	// Components
	UPROPERTY(VisibleAnywhere, Category = "Collision")
	class UCapsuleComponent *CapsuleComponent;

	UPROPERTY(EditAnywhere, Category = "Unit Mesh")
	class UStaticMeshComponent *UnitMesh;

	UPROPERTY(VisibleAnywhere, Category = "Selection")
	class UDecalComponent *SelectionDecal;

	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UFloatingPawnMovement *MovementComponent;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent *HealthBarWidget;

	// AI
	class AAIController *AIController;

	UFUNCTION()
	void OnRep_Health();

	// Damage & death
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	void UpdateHealthBar();
	virtual void Die();
};
