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
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool trapActivated() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		void startCombat() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		void stopCombat() const;
protected:
	virtual void BeginPlay() override;
private:
	///UFUNCTION(Category = Callbacks)
	///	void onOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	///		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	///		const FHitResult & SweepResult);
	///UFUNCTION(Category = Callbacks)
	///	void onOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	///		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
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
};
