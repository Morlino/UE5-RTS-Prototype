// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSUnit.generated.h"

UCLASS()
class RTSPROJECT_API ARTSUnit : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARTSUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetSelected(bool bSelected);

private:
	UPROPERTY(EditAnywhere, Category = "Unit Mesh")
	class UStaticMeshComponent *UnitMesh;

	UPROPERTY(VisibleAnywhere, Category = "Selection")
	class UDecalComponent *SelectionDecal;

	float Health = 400.0f;
	float MaxHealth = 400.0f;
	float MovementSpeed = 100.0f;

	virtual void MoveToLocation(FVector Target);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser) override;
	virtual void Die();
};
