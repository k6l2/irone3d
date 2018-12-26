// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Irone3DPlayer.generated.h"
UCLASS()
class IRONE3D_API AIrone3DPlayer : public ACharacter
{
	GENERATED_BODY()
private:
	static const FLinearColor HURT_OUTLINE_COLOR;
public:
	AIrone3DPlayer();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, 
		FDamageEvent const& DamageEvent, AController * EventInstigator,
		AActor * DamageCauser) override;
    void moveForward(float value);
    void moveRight(float value);
    void turnRate(float value);
    void lookUpRate(float value);
    UFUNCTION(BlueprintCallable, Category=Animation)
		float lateralSpeed() const;
    UFUNCTION(BlueprintCallable, Category=Animation)
		float verticalSpeed() const;
    // Positive verticalVelocity1D values mean you are moving "upwards"
    UFUNCTION(BlueprintCallable, Category=Animation)
		float verticalVelocity1D() const;
    UFUNCTION(BlueprintCallable, Category=Animation)
		bool isFalling() const;
    UFUNCTION(BlueprintCallable, Category=Animation)
		void hardLandingStart();
    UFUNCTION(BlueprintCallable, Category=Animation)
		void hardLandingEnd();
    UFUNCTION(BlueprintCallable, Category=Animation)
		void canAttackStart();
    UFUNCTION(BlueprintCallable, Category=Animation)
		void canAttackEnd();
    UFUNCTION(BlueprintCallable, Category=Animation)
		bool isSleeping() const;
	void setSleeping(bool value);
    void tryAttackStart();
    void tryAttackEnd();
    void dashStart();
    void dashEnd();
    UFUNCTION(BlueprintCallable, Category=Animation)
		bool isTryingToAttack() const;
    UFUNCTION(BlueprintCallable, Category=Animation)
		void attackStart();
    UFUNCTION(BlueprintCallable, Category=Animation)
		void attackEnd();
    UFUNCTION(BlueprintCallable, Category=Animation)
		void updateAttackAnimationProgress(float value);
	UFUNCTION(BlueprintCallable, Category = Player)
		bool isKilled() const;
	void copyCameraPropertiesTo(ACameraActor*const otherCam) const;
	void cutSceneStopVelocity();
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION()
		void onCapsuleHit(UPrimitiveComponent* HitComponent,
			AActor* OtherActor, UPrimitiveComponent* OtherComp,
			FVector NormalImpulse, const FHitResult& Hit);
	///UFUNCTION()
	///	void onAttackOverlap(UPrimitiveComponent* OverlappedComponent,
	///		AActor* OtherActor, UPrimitiveComponent* OtherComp,
	///		int32 OtherBodyIndex, bool bFromSweep,
	///		const FHitResult & SweepResult);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, 
			meta=(AllowPrivateAccess = "true"))
		class USpringArmComponent* cameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, 
			meta=(AllowPrivateAccess = "true"))
		class UCameraComponent* camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, 
			meta=(AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* meshCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, 
			meta=(AllowPrivateAccess = "true"))
		class UStaticMeshComponent* meshAttack;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Components, 
			meta=(AllowPrivateAccess = "true"))
		class UCombatComponent* attackCombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UUnitComponent* unitComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class UParticleSystemComponent* dashParticleComponent;
    UPROPERTY()
		UMaterialInstanceDynamic* dynMaterialSlash;
	UPROPERTY(EditDefaultsOnly, Category = Player)
		TSubclassOf<AActor> classLockedDoor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Combat, 
			meta = (AllowPrivateAccess = "true"))
		UParticleSystem* particleSystemBlood;
	UPROPERTY(EditDefaultsOnly, Category = Player)
		class USoundCue* sfxAttack;
	UPROPERTY(EditDefaultsOnly, Category = Player)
		class USoundCue* sfxHit;
	UMaterialInstanceDynamic* outlineMaterial;
	UMaterialInstanceDynamic* eyesMaterial;
	FLinearColor defaultOutlineColor;
    float attackAnimationProgress;
	float hurtFlashSeconds = 0.f;
    bool hardLanding;
    // canAttack can only be true if we're in idle/walk/run state
    bool canAttack;
    bool tryingAttack;
    bool dashing = false;
    bool attackAnimationPlaying;
	UPROPERTY(EditAnywhere, Category = Player)
		bool sleeping = false;
};
