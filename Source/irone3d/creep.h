// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "creep.generated.h"
UCLASS()
class IRONE3D_API Acreep : public ACharacter
{
	GENERATED_BODY()
private:
	static const FLinearColor HURT_OUTLINE_COLOR;
	static const float HURT_FLASH_SECONDS;
public:
	Acreep();
    UFUNCTION(BlueprintCallable, Category = Animation)
    float lateralSpeed() const;
    bool canSee(AActor* actor) const;
    virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController * EventInstigator, AActor * DamageCauser) override;
	void getOverlappingAggroActors(TArray<AActor*>& outArray) const;
	float getPushAwaySeconds() const;
protected:
	virtual void BeginPlay() override;
private:
    UFUNCTION(Category = Sensory)
		void onSeePawn(APawn* pawn);
    UFUNCTION()
		void onUnitDie();
private:
	UPROPERTY(VisibleAnywhere, Transient, Category = Creep)
		float pushAwaySeconds = 0.f;
	UPROPERTY(VisibleAnywhere, Transient, Category = Creep)
		float hurtFlashSeconds = 0.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UPawnSensingComponent* pawnSense;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UUnitComponent* unitComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, 
			meta = (AllowPrivateAccess = "true"))
		UParticleSystem* particleSystemBlood;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class USphereComponent* componentAttackSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class USphereComponent* componentAggroSphere;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components, 
			meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* componentCombat;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxHit;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxDestroyed;
	UPROPERTY(EditDefaultsOnly, Category = Creep)
		TSubclassOf<class AActor> bpItemClass;
	UMaterialInstanceDynamic* outlineMaterial;
	UMaterialInstanceDynamic* eyesMaterial;
	FLinearColor defaultOutlineColor;
};
