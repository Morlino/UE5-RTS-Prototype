// Fill out your copyright notice in the Description page of Project Settings.


#include "RTSUnit.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/DecalComponent.h"
#include "AIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/WidgetComponent.h"
#include "HealthBarWidget.h"

// Sets default values
ARTSUnit::ARTSUnit()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleRoot"));
	RootComponent = CapsuleComponent;
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponent->SetCollisionObjectType(ECC_Pawn);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
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
	MovementComponent->MaxSpeed = Stats.MovementSpeed;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(RootComponent);
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HealthBarWidget->SetDrawSize(FVector2D(100.f, 20.f));
	HealthBarWidget->SetRelativeLocation(FVector(0.f, 0.f, 120.f));

	// Load the Blueprint class
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarBPClass(TEXT("/Game/Blueprints/UI/WBP_HealthProgressBar"));
	if (HealthBarBPClass.Succeeded())
	{
		HealthBarWidget->SetWidgetClass(HealthBarBPClass.Class);
	}
}


// Called every frame
void ARTSUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
		case EUnitState::Idle:
			// Nothing to do
			break;

		case EUnitState::Moving:
			UpdateMovement(DeltaTime);
			break;

		case EUnitState::Attacking:
			UpdateAttack(DeltaTime);
			break;

		case EUnitState::Following:
			UpdateFollow(DeltaTime);
			break;
	}
}


void ARTSUnit::SetSelected(bool bSelected)
{
	SelectionDecal->SetHiddenInGame(!bSelected);
}

void ARTSUnit::MoveTo(const FVector &TargetLocation)
{
	MoveTarget = TargetLocation;
	CurrentState = EUnitState::Moving;
	bIsMovingToTarget = true;
	
	if (AIController)
	{
		float AcceptanceRadius = 50.0f;
		AIController->MoveToLocation(TargetLocation, AcceptanceRadius);
	}
}

void ARTSUnit::Follow(const ARTSUnit *TargetUnit)
{
	if (!TargetUnit)
		return;

	CurrentTarget = const_cast<ARTSUnit *>(TargetUnit);
	CurrentState = EUnitState::Following;
}

void ARTSUnit::StartAttack(ARTSUnit *TargetUnit)
{
	if (!TargetUnit || TargetUnit == this)
		return;

	CurrentTarget = TargetUnit;
	CurrentState = EUnitState::Attacking;
	bIsMovingToTarget = false;
}

void ARTSUnit::Attack(ARTSUnit *TargetUnit)
{
	if (!TargetUnit || TargetUnit == this)
		return;

	UE_LOG(LogTemp, Warning, TEXT("Attacking %s"), *TargetUnit->GetName());

	UGameplayStatics::ApplyDamage(TargetUnit, Stats.AttackDamage, GetController(), this, UDamageType::StaticClass());
}

void ARTSUnit::StopAttack()
{
	CurrentTarget = nullptr;
}

// Called when the game starts or when spawned
void ARTSUnit::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	UpdateHealthBar();
}

void ARTSUnit::UpdateMovement(float DeltaTime)
{
	if (!bIsMovingToTarget || !AIController)
		return;

	float Distance = FVector::Dist(GetActorLocation(), MoveTarget);

	if (Distance <= 10.0f) // small arrival threshold
	{
		AIController->StopMovement();
		CurrentState = EUnitState::Idle;
		bIsMovingToTarget = false;
	}
}
void ARTSUnit::UpdateAttack(float DeltaTime)
{
	if (!CurrentTarget || CurrentTarget->Stats.Health <= 0 || !IsValid(CurrentTarget))
	{
		StopAttack();
		CurrentState = EUnitState::Idle;
		return;
	}

	float Distance = FVector::Dist(GetActorLocation(), CurrentTarget->GetActorLocation());
	float EffectiveAttackRange = Stats.AttackRange + GetSimpleCollisionRadius() + CurrentTarget->GetSimpleCollisionRadius();

	if (Distance > EffectiveAttackRange)
	{
		if (!bIsMovingToTarget)
		{
			AIController->MoveToActor(CurrentTarget, Stats.AttackRange);
			bIsMovingToTarget = true;
		}
	}
	else
	{
		if (bIsMovingToTarget && AIController)
		{
			AIController->StopMovement();
			bIsMovingToTarget = false;
		}

		// Attack logic
		TimeSinceLastAttack += DeltaTime;
		float AttackInterval = 1.0f / Stats.AttackSpeed;
		if (TimeSinceLastAttack >= AttackInterval)
		{
			Attack(CurrentTarget);
			TimeSinceLastAttack = 0.0f;
		}
	}
}

void ARTSUnit::UpdateFollow(float DeltaTime)
{
	if (!CurrentTarget || !AIController)
		return;

	// Adjust the distance based on Target Size
	const FBox TargetBox = CurrentTarget->GetComponentsBoundingBox();
	const float TargetRadius = TargetBox.GetExtent().Size2D();
	const float FollowDistance = TargetRadius + 50.0f; // 50 units extra buffer

	AIController->MoveToActor(CurrentTarget, FollowDistance);
}

float ARTSUnit::TakeDamage(float DamageAmount, FDamageEvent const &DamageEvent, AController *EventInstigator, AActor *DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("%s takes %f damage"), *GetName(), ActualDamage);

	Stats.Health -= ActualDamage;
	Stats.Health = FMath::Clamp(Stats.Health, 0.0f, Stats.MaxHealth);

	// Update floating HP bar
	UpdateHealthBar();

	if (Stats.Health <= 0.0f)
	{
		Die();
	}

	return ActualDamage;
}

void ARTSUnit::UpdateHealthBar()
{
	if (!HealthBarWidget)
		return;

	if (UUserWidget *Widget = HealthBarWidget->GetUserWidgetObject())
	{
		if (UHealthBarWidget *HealthWidget = Cast<UHealthBarWidget>(Widget))
		{
			HealthWidget->UpdateHealth(Stats.Health / Stats.MaxHealth);
		}
	}
}

void ARTSUnit::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Unit Died"));
	Destroy();
}
