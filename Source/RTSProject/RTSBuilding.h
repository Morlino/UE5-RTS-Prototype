// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RTSBuilding.generated.h"

UCLASS()
class RTSPROJECT_API ARTSBuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARTSBuilding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent *StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent *CollisionComponent;

	UPROPERTY(Replicated, BlueprintReadWrite, EditAnywhere)
	int32 TeamID;
	
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health = 1500.0f;

	UPROPERTY(ReplicatedUsing = OnRep_BuildProgress)
	float BuildProgress;

	UPROPERTY(Replicated)
	bool bIsConstructed;

	float MaxHealth = 1500.0f;
	float BuildTime = 12.0f;
	int32 MetalCost = 50;
	int32 FuelCost = 0;
	int32 SupplyCost = 1;
	FVector2D Size = FVector2D(4.0f, 4.0f);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	int32 GetTeamID() const;

	void SetTeamID(int32 NewTeamID);

	virtual float TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser) override;

	UFUNCTION()
	void OnRep_Health();

	UFUNCTION()
	void OnRep_BuildProgress();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;
};
