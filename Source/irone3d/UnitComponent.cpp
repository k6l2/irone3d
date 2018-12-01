// YOLO SWAG 420
#include "UnitComponent.h"
#include "CombatComponent.h"
#include "Runtime/Engine/Classes/Components/PrimitiveComponent.h"
UUnitComponent::UUnitComponent()
	:hitpoints(1.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UUnitComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	auto const owner = GetOwner();
	if (!ensure(owner))
	{
		return;
	}
	//if (collidingCombatComponents.Num() > 0)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("collidingCombatComponents.Num()=%i"),
	//		collidingCombatComponents.Num());
	//}
	// Need to copy into a temp array here because elements
	//	get removed while inside loop, invalidating iterators
	TArray<class UCombatComponent*> tempCollidingCombatComps =
		collidingCombatComponents;
	for (auto combatComp : tempCollidingCombatComps)
	{
		if (hitpoints <= 0)
		{
			break;
		}
		if (!combatComp->isAttackActive() ||
			!combatComp->registerHit(this))
		{
			continue;
		}
		const float damage = combatComp->getDamage();
		auto combatOwner = combatComp->GetOwner();
		auto instigator = combatOwner->GetInstigatorController();
		hitpoints -= damage;
		//UE_LOG(LogTemp, Warning, TEXT("%f damage dealt! hitpoints=%f"),
		//	damage, hitpoints);
		FDamageEvent dmgEvent;
		owner->TakeDamage(damage, dmgEvent, instigator, combatOwner);
		if (hitpoints <= 0)
		{
			delegateDie.ExecuteIfBound();
			if (destroyOnDie)
			{
				owner->Destroy();
			}
		}
		if (combatComp->destroyOwnerOnDamageDealt())
		{
			combatOwner->Destroy();
		}
	}
}
void UUnitComponent::addVulnerablePrimitiveComponent(UPrimitiveComponent* comp)
{
	if (hasBegunPlay)
	{
		bindOverlapsToComponent(comp);
	}
	vulnerableComponents.Add(comp);
}
void UUnitComponent::setHitpoints(float hp)
{
	hitpoints = hp;
}
bool UUnitComponent::isDead() const
{
	return hitpoints <= 0;
}
void UUnitComponent::setDestroyOnDie(bool value)
{
	destroyOnDie = value;
}
void UUnitComponent::onUnitOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult & SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("overlap Begin"));
	// We need to make sure that othercomp contains a UCombatComponent //
	// If it does contain a UCombatComponent, we need to keep tabs on it //
	UCombatComponent* otherCombatComp = findCombatComponent(OtherComp);
	if (otherCombatComp)
	{
		if (collidingCombatComponents.Contains(otherCombatComp))
		{
			return;
		}
		//UE_LOG(LogTemp, Warning, TEXT("CombatComp overlapped!"));
		collidingCombatComponents.Add(otherCombatComp);
	}
}
void UUnitComponent::onUnitOverlapEnd(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	UCombatComponent* otherCombatComp = findCombatComponent(OtherComp);
	if (otherCombatComp)
	{
		collidingCombatComponents.Remove(otherCombatComp);
	}
}
void UUnitComponent::bindOverlapsToComponent(UPrimitiveComponent* component)
{
	if (!component->OnComponentBeginOverlap.IsAlreadyBound(
		this, &UUnitComponent::onUnitOverlapBegin))
	{
		component->OnComponentBeginOverlap.AddDynamic(
			this, &UUnitComponent::onUnitOverlapBegin);
		component->OnComponentEndOverlap.AddDynamic(
			this, &UUnitComponent::onUnitOverlapEnd);
	}
}
void UUnitComponent::BeginPlay()
{
	Super::BeginPlay();
	for (auto comp : vulnerableComponents)
	{
		bindOverlapsToComponent(comp);
	}
	hasBegunPlay = true;
}
UCombatComponent * UUnitComponent::findCombatComponent(UPrimitiveComponent * otherComp)
{
	TArray < USceneComponent * > otherCompChildren;
	otherComp->GetChildrenComponents(false, otherCompChildren);
	for (auto comp : otherCompChildren)
	{
		auto combatComp = Cast<UCombatComponent>(comp);
		if (combatComp)
		{
			return combatComp;
		}
	}
	return nullptr;
}
