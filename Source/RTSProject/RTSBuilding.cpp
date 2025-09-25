// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSBuilding.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ARTSBuilding::ARTSBuilding()
{
	PrimaryActorTick.bCanEverTick = false;

	// Server Replicate
	bReplicates = true;
	SetReplicateMovement(true);

	Tags.Add(FName("Building"));

	// Root collision used for navigation
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->SetBoxExtent(FVector(200.f, 200.f, 300.f));
	CollisionComponent->SetCollisionProfileName(TEXT("BlockAll"));
	CollisionComponent->SetCanEverAffectNavigation(true);
	CollisionComponent->SetMobility(EComponentMobility::Movable); // important for navmesh

	// Visual mesh
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComponent->SetupAttachment(RootComponent);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision); // visual only
}

// Called when the game starts or when spawned
void ARTSBuilding::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARTSBuilding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 ARTSBuilding::GetTeamID() const
{
	return TeamID;
}

void ARTSBuilding::SetTeamID(int32 NewTeamID)
{
	TeamID = NewTeamID;
}

float ARTSBuilding::TakeDamage(float DamageAmount, const FDamageEvent &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	Health -= DamageAmount;
	if (Health <= 0)
	{
		// Handle destruction
	}
	OnRep_Health();
	return DamageAmount;
}

void ARTSBuilding::OnRep_Health()
{
	// Do some logic here
}

void ARTSBuilding::OnRep_BuildProgress()
{
	// Do some logic here
}

void ARTSBuilding::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone
	DOREPLIFETIME(ARTSBuilding, TeamID);
	DOREPLIFETIME(ARTSBuilding, Health);
	DOREPLIFETIME(ARTSBuilding, BuildProgress);
	DOREPLIFETIME(ARTSBuilding, bIsConstructed);
}