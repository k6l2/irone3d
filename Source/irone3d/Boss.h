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
	virtual void SetupPlayerInputComponent(
		class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class USphereComponent* componentSphere;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components,
			meta = (AllowPrivateAccess = "true"))
		class USkeletalMeshComponent* componentMesh;
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
		FVector velocity;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		FVector prevLocation;
	UPROPERTY(VisibleAnywhere, Category = Boss)
		TWeakObjectPtr<APawn> targetPawn;
};
/*
	IDLE -> Choose new patrol location -> Spawn orb time
*/
