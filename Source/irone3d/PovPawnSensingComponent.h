// YOLO SWAG 420
/*
	This class allows an Actor to be able to attach a PawnSensingComponent
	to a socket on a mesh, as long as they override the function
	virtual void GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const

	Derived from this thread:
	https://answers.unrealengine.com/questions/197094/pawn-sensing-to-socket.html
*/
#pragma once
#include "CoreMinimal.h"
#include "Perception/PawnSensingComponent.h"
#include "PovPawnSensingComponent.generated.h"
UCLASS()
class IRONE3D_API UPovPawnSensingComponent : public UPawnSensingComponent
{
	GENERATED_BODY()
public:
	virtual FVector GetSensorLocation() const override;
	virtual FRotator GetSensorRotation() const override;
};
