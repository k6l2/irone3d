// YOLO SWAG 420
#include "CombatComponent.h"
#include "UnitComponent.h"
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
float UCombatComponent::getDamage() const
{
	return damage;
}
void UCombatComponent::startAttack(float newDamage)
{
	damage = newDamage;
	attackActive = true;
	hitUnits.Empty();
}
bool UCombatComponent::registerHit(UUnitComponent * unitComp)
{
	if (hitUnits.Contains(unitComp) && !alwaysAttacking)
	{
		///UE_LOG(LogTemp, Warning, TEXT("registerHit->false! alwaysAttacking=%s"), 
		///	(alwaysAttacking ? TEXT("T"):TEXT("F")));
		return false;
	}
	hitUnits.Add(unitComp);
	return true;
}
void UCombatComponent::stopAttack()
{
	attackActive = false;
	alwaysAttacking = false;
	hitUnits.Empty();
}
bool UCombatComponent::isAttackActive() const
{
	return attackActive || alwaysAttacking;
}
void UCombatComponent::setAlwaysAttacking(bool value)
{
	alwaysAttacking = value;
	hitUnits.Empty();
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}
