// YOLO SWAG 420
/*
	Only create ONE instance per actor.
	Attach this component to an actor and assign it to physical scene components
	to give this actor the ability to receive combat events from CombatComponents

	This class handles all the following:
		-hitpoints
		-Damage events
		-Damage cooldowns (i-frames)
		-resistences/defenses
	This component does not need to handle factions, since Unreal Engine's
	collision channels can automatically handle this.
*/
#pragma once
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitComponent.generated.h"
///DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitHitDelegate, UObject*, attackingObject);
DECLARE_DELEGATE(FUnitDieDelegate);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IRONE3D_API UUnitComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UUnitComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	// to use the Unit component, call this function once in the owning Actor's 
	//	constructor, or if not possible due to the desired vulnerable 
	//	components not being  initialized in the constructor, call it once in 
	//	BeginPlay
	void addVulnerablePrimitiveComponent(UPrimitiveComponent* comp);
	void setHitpoints(float hp);
	bool isDead() const;
	float getGlobalHitInvincibleDuration() const;
	void setInvulnerable(bool value);
	void setImmortal(bool value);
protected:
	virtual void BeginPlay() override;
private:
	void bindOverlapsToComponent(UPrimitiveComponent* component);
	UFUNCTION()
		void onUnitOverlapBegin(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep,
			const FHitResult & SweepResult);
	UFUNCTION()
		void onUnitOverlapEnd(UPrimitiveComponent* OverlappedComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex);
	class UCombatComponent* findCombatComponent(UPrimitiveComponent * otherComp);
public:
	FUnitDieDelegate delegateDie;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Unit,
			meta = (AllowPrivateAccess = "true"))
		float hitpoints;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Unit, 
			meta = (AllowPrivateAccess = "true"))
		float globalHitInvincibleDuration = 0.f;
	UPROPERTY(VisibleInstanceOnly, Transient, Category = Unit)
		float globalHitInvincibleTimer = 0.f;
	UPROPERTY(VisibleInstanceOnly, Transient, Category = Unit)
		TArray<class UCombatComponent*> collidingCombatComponents;
	UPROPERTY(VisibleAnywhere, Category = Unit)
		TArray<class UPrimitiveComponent*> vulnerableComponents;
	UPROPERTY(VisibleAnywhere, Category = Unit)
		// if this is set to true, combat events are not even considered for
		//	this unit.
		bool invulnerable = false;
	UPROPERTY(VisibleAnywhere, Category = Unit)
		// Unlike the invulnerable flag, this will allow damage events to 
		//	occur, while still preventing the unit from losing hitpoints.
		//	delegateDie is also guaranteed to never be called if this is set
		bool immortal = false;
	UPROPERTY(Transient)
		// This variable is a hack that will allow us to call 
		//	addVulnerablePrimitiveComponent in BeginPlay instead of the
		//	Actor's constructor, in case we derive from Character and need to
		//	add the Character's capsule component to the list of vulnerable 
		//	components...
		bool hasBegunPlay = false;
};
