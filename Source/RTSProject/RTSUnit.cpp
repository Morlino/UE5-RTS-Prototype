// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSUnit.h"
#include "Components/DecalComponent.h"

// Sets default values
ARTSUnit::ARTSUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	UnitMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UnitMesh"));
	UnitMesh->SetupAttachment(RootComponent);

	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SelectionDecal"));
	SelectionDecal->SetupAttachment(RootComponent);
	SelectionDecal->DecalSize = FVector(64.0f, 128.0f, 128.0f);
	SelectionDecal->SetHiddenInGame(true);
	SelectionDecal->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
}

// Called when the game starts or when spawned
void ARTSUnit::BeginPlay()
{
	Super::BeginPlay();
	
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

void ARTSUnit::MoveToLocation(FVector Target)
{

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
