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
}
void ATurretLaser::setAim(const FVector& directionNormal)
{
	static const float LASER_SPEED = 100.f;
	velocity = directionNormal * LASER_SPEED;
}
