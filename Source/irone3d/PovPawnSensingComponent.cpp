// YOLO SWAG 420
#include "PovPawnSensingComponent.h"
#include <GameFramework/Actor.h>
FVector UPovPawnSensingComponent::GetSensorLocation() const
{
	FVector  location{ FVector::ZeroVector };
	FRotator rotation{ FRotator::ZeroRotator };
	const AActor* const sensorActor = GetSensorActor();
	if (sensorActor)
	{
		sensorActor->GetActorEyesViewPoint(location, rotation);
	}
	return location;
}
FRotator UPovPawnSensingComponent::GetSensorRotation() const
{
	FVector  location{ FVector::ZeroVector };
	FRotator rotation{ FRotator::ZeroRotator };
	const AActor* const sensorActor = GetSensorActor();
	if (sensorActor)
	{
		sensorActor->GetActorEyesViewPoint(location, rotation);
	}
	return rotation;
}
