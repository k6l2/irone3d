// YOLO SWAG 420
#include "CombatComponent.h"
#include "UnitComponent.h"
UCombatComponent::UCombatComponent()
	:attackActive(false)
	,damage(1.f)
{
	PrimaryComponentTick.bCanEverTick = false;
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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
	if (hitUnits.Contains(unitComp))
	{
		return false;
	}
	hitUnits.Add(unitComp);
	return true;
}
void UCombatComponent::stopAttack()
{
	attackActive = false;
	hitUnits.Empty();
}
bool UCombatComponent::isAttackActive() const
{
	return attackActive;
}
bool UCombatComponent::destroyOwnerOnDamageDealt() const
{
	return m_destroyOwnerOnDamageDealt;
}
void UCombatComponent::setDestroyOnDealDamage(bool value)
{
	m_destroyOwnerOnDamageDealt = value;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
}
