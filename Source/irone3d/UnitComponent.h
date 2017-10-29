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
//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUnitHitDelegate, UObject*, attackingObject);
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IRONE3D_API UUnitComponent : public UActorComponent
{
	GENERATED_BODY()
public:
	UUnitComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	// These cannot be hooked to OnComponentBegin/EndOverlap delegates 
	//	for whatever reason because of some weird binding error
	void onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
		const FHitResult & SweepResult);
	void onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
protected:
	virtual void BeginPlay() override;
private:
	class UCombatComponent* findCombatComponent(UPrimitiveComponent * otherComp);
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float hitpoints;
	//UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	//float globalHitInvincibleTime;
	//float globalHitInvincibleTimer;
	TArray<class UCombatComponent*> collidingCombatComponents;
};
