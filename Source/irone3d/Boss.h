// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Boss.generated.h"
UCLASS()
class IRONE3D_API ABoss : public APawn
{
	GENERATED_BODY()
public:
	ABoss();
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount,
		FDamageEvent const& DamageEvent, AController * EventInstigator,
		AActor * DamageCauser) override;
	UFUNCTION(BlueprintCallable, Category = BossAnimation)
		float getSpeed2D() const;
	UFUNCTION(BlueprintCallable, Category = BossAnimation)
		bool chargingOrb() const;
	UFUNCTION(BlueprintCallable, Category = BossAnimation)
		bool isFalling() const;
	UFUNCTION(BlueprintCallable, Category = BossAnimation)
		bool isDead() const;
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
		void onUnitDie();
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class USphereComponent* componentSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* componentMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class UUnitComponent* componentUnit;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector patrolAreaMin;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector patrolAreaMax;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector patrolDestination;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		float idleTime;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		float chargeOrbTime;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		float orbAttackCooldownTime;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		bool hoverUp;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		bool reachedPatrolLocation;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		bool falling;
	UPROPERTY(EditAnywhere, Category = Boss)
		bool gravityOn;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector velocity;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector prevLocation;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		TWeakObjectPtr<APawn> targetPawn;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		TWeakObjectPtr<class ABossOrb> orb;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxHit;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxMagicCharge;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxMagicRelease;
};
