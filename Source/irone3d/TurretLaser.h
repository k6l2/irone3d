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
	// Sets default values for this actor's properties
	ATurretLaser();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
