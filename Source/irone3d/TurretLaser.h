// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TurretLaser.generated.h"
UCLASS()
class IRONE3D_API ATurretLaser : public AActor
{
	GENERATED_BODY()
public:
	ATurretLaser();
	virtual void Tick(float DeltaTime) override;
	void setAim(const FVector& directionNormal);
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, 
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UStaticMeshComponent* meshComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
		Category = Components, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* combatComponent;
	FVector velocity;
};
