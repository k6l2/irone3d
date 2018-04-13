// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableFloor.generated.h"
UCLASS()
class IRONE3D_API ABreakableFloor : public AActor
{
	GENERATED_BODY()
public:	
	ABreakableFloor();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
private:
};
