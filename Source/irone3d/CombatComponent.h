// YOLO SWAG 420
/*
	Attach this component to scene components that only overlap actors.
	Once they overlap, this component shall send combat events to the UnitComponent
	so that damage events can get generated and handled.

	This component does not need to handle factions, since Unreal Engine's
	collision channels can automatically handle this.
*/
#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CombatComponent.generated.h"
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class IRONE3D_API UCombatComponent : public USceneComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;
	float getDamage() const;
	void startAttack();
	// returns true if the hit has been registered for the first time
	//	false if this CombatComponent has already hit unitComp during this attack
	bool registerHit(class UUnitComponent* unitComp);
	void stopAttack();
	bool isAttackActive() const;
protected:
	virtual void BeginPlay() override;
private:
	bool attackActive;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
	float damage;
	TArray<class UUnitComponent*> hitUnits;
};
