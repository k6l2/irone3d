// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include <GameFramework/Pawn.h>
#include "Turret.generated.h"
UCLASS()
class IRONE3D_API ATurret : public APawn
{
	GENERATED_BODY()
public:	
	ATurret();
	virtual void Tick(float deltaSeconds) override;
	UFUNCTION(BlueprintCallable, Category = Animation)
		float yaw() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		float pitch() const;
	UFUNCTION(BlueprintCallable, Category = Animation)
		bool active() const;
	virtual void GetActorEyesViewPoint(
		FVector& OutLocation, FRotator& OutRotation) const override;
protected:
	virtual void BeginPlay() override;
private:
	UFUNCTION(Category = Sensory)
		void onSeePawn(APawn* pawn);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Mesh, meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* skeletalMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Combat, meta = (AllowPrivateAccess = "true"))
		class UPovPawnSensingComponent* pawnSense;
	FVector2D yawVector;
};
