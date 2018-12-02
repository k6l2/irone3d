// YOLO SWAG 420
#pragma once
#define ECC_EnemyPawn  ECollisionChannel::ECC_GameTraceChannel2
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
    virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
		AController * EventInstigator, AActor * DamageCauser) override;
protected:
	virtual void BeginPlay() override;
private:
    UFUNCTION(Category = Sensory)
    void onSeePawn(APawn* pawn);
private:
	float hurtFlashSeconds;
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
	UMaterialInstanceDynamic* outlineMaterial;
	UMaterialInstanceDynamic* eyesMaterial;
	FLinearColor defaultOutlineColor;
};
