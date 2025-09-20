// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSUnit.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "AIController.h"
#include "DrawDebugHelpers.h"

// Call this when you want to visualize a target location
void DrawTargetDebugSphere(UWorld *World, const FVector &TargetLocation, float Radius = 50.f, FColor Color = FColor::Green, float Duration = 0.5f)
{
	if (!World)
		return;

	DrawDebugSphere(
		World,
		TargetLocation, // Center of the sphere
		Radius,			// Radius
		12,				// Segments (higher = smoother)
		Color,			// Color
		false,			// Persistent lines
		Duration,		// LifeTime in seconds
		0,				// Depth priority
		2.f				// Line thickness
	);
}

// Sets default values
ARTSUnit::ARTSUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleRoot"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->InitCapsuleSize(42.0f, 96.0f);
	CapsuleComponent->SetCanEverAffectNavigation(false);

	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(RootComponent);
	UnitMesh->SetRelativeLocation(FVector(0.0f, 0.0f, -CapsuleComponent->GetScaledCapsuleHalfHeight()));
	UnitMesh->SetCanEverAffectNavigation(false);

	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecal->SetupAttachment(RootComponent);
	SelectionDecal->DecalSize = FVector(64.0f, 128.0f, 128.0f);
	SelectionDecal->SetHiddenInGame(true);
	SelectionDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
    MovementComponent->UpdatedComponent = RootComponent;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

// Called when the game starts or when spawned
void ARTSUnit::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());
}

void ARTSUnit::SetSelected(bool bSelected)
{
	SelectionDecal->SetHiddenInGame(!bSelected);
}

// Called every frame
void ARTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARTSUnit::MoveToLocation(const FVector &TargetLocation)
{
	if (AIController)
	{
		DrawTargetDebugSphere(GetWorld(), TargetLocation);
		AIController->MoveToLocation(TargetLocation);
	}
}

void ARTSUnit::Attack(ARTSUnit *Unit)
{
	UE_LOG(LogTemp, Warning, TEXT("Attacking"));
}

float ARTSUnit::TakeDamage(float DamageAmount, struct FDamageEvent const &DamageEvent, class AController *EventInstigator, AActor *DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Health -= ActualDamage;
	if (Health <= 0.0f)
	{
		Die();
	}

	return ActualDamage;
}
void ARTSUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Unit Died"));
}
