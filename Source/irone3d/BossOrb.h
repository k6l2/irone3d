// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BossOrb.generated.h"
UCLASS()
class IRONE3D_API ABossOrb : public AActor
{
	GENERATED_BODY()
public:	
	ABossOrb();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent, AController * EventInstigator,
		AActor * DamageCauser) override;
	void shootAt(APawn* targetPawn, bool reflectable);
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
		void onDamageDealt();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class USphereComponent* componentSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* componentMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* componentCombat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class UUnitComponent* componentUnit;
	UPROPERTY(VisibleAnywhere, Category = BossOrb)
		FVector velocity;
	UPROPERTY(VisibleAnywhere, Category = BossOrb)
		float bounceSeconds;
	UPROPERTY(VisibleAnywhere, Category = BossOrb)
		float flyAwaySeconds;
	UPROPERTY(VisibleAnywhere, Category = BossOrb)
		bool isReflectable;
	UPROPERTY(VisibleAnywhere, Category = BossOrb)
		class UMaterialInstanceDynamic* materialUnreflectable;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxHitWall;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxReflectAttack;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxDestroyed;
	UPROPERTY(EditDefaultsOnly, Category = BossOrb)
		UParticleSystem* particleSystemExplode;
};
