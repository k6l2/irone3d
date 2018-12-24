// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpikeTrap.generated.h"
UCLASS()
class IRONE3D_API ASpikeTrap : public AActor
{
	GENERATED_BODY()
public:	
	ASpikeTrap();
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool trapActivated() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		void startCombat() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		void stopCombat() const;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* combatComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* skeletalMeshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* boxComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* boxComponentRoot;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Combat, meta = (AllowPrivateAccess = "true"))
		class USceneComponent* sceneComponent;
	UPROPERTY(VisibleAnywhere, Category = Combat)
		float activationTimer;
	UPROPERTY(EditDefaultsOnly, Category = SoundEffects)
		class USoundCue* sfxSpike;
	///UPROPERTY(VisibleAnywhere, Category = SpikeTrap)
	///	bool justTriggered = false;
};
