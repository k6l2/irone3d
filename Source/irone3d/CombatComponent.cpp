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
void UCombatComponent::startAttack()
{
	attackActive = true;
	hitUnits.Empty();
}
bool UCombatComponent::registerHit(UUnitComponent * unitComp)
{
	if (hitUnits.Contains(unitComp) && !alwaysAttacking)
	{
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
bool UCombatComponent::destroyOwnerOnDamageDealt() const
{
	return m_destroyOwnerOnDamageDealt;
}
void UCombatComponent::setDestroyOnDealDamage(bool value)
{
	m_destroyOwnerOnDamageDealt = value;
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
