// YOLO SWAG 420
#include "TurretLaser.h"
#include "CombatComponent.h"
#include <Runtime/Engine/Classes/Components/StaticMeshComponent.h>
ATurretLaser::ATurretLaser()
	: meshComponent(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComponent")))
	, combatComponent(CreateDefaultSubobject<UCombatComponent>(TEXT("combatComponent")))
{
	RootComponent = meshComponent;
	combatComponent->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}
void ATurretLaser::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetActorLocation(GetActorLocation() + velocity * DeltaTime);
	static const float RANGE = 5000.f;
	if ((GetActorLocation() - spawnLocation).Size() >= RANGE)
	{
		Destroy();
	}
}
void ATurretLaser::setAim(const FVector& directionNormal)
{
	static const float LASER_SPEED = 750.f;
	velocity = directionNormal * LASER_SPEED;
	SetActorRotation(directionNormal.Rotation());
	// move the laser up the turret's barrel a bit 
	//	so it doesn't pop out the back when fired
	SetActorLocation(GetActorLocation() + directionNormal * 100);
}
void ATurretLaser::BeginPlay()
{
	Super::BeginPlay();
	combatComponent->startAttack();
	spawnLocation = GetActorLocation();
}
