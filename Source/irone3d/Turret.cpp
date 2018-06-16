// YOLO SWAG 420
#include "Turret.h"
#include "PovPawnSensingComponent.h"
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h>
//#include <Runtime/AIModule/Classes/Perception/PawnSensingComponent.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
ATurret::ATurret()
	:skeletalMesh(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skeletalMesh")))
	, pawnSense  (CreateDefaultSubobject<UPovPawnSensingComponent> (TEXT("pawnSense")))
	, yawVector(1.f, 0.f)
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATurret::Tick(float deltaSeconds)
{
	static const float YAW_PATROL_ROTATION_RATE = 100.f;
	yawVector = yawVector.GetRotated(
		YAW_PATROL_ROTATION_RATE*deltaSeconds);
}
float ATurret::yaw() const
{
	const float retVal = FMath::Atan2(yawVector.Y, yawVector.X) * 180 / PI;
	return retVal;
}
float ATurret::pitch() const
{
	///TODO
	return 0.f;
}
bool ATurret::active() const
{
	///TODO
	return false;
}
void ATurret::GetActorEyesViewPoint(
	FVector& OutLocation, FRotator& OutRotation) const
{
	const USkeletalMeshSocket* const socketSensor = skeletalMesh ?
		skeletalMesh->GetSocketByName("socketPawnSensor") : nullptr;
	if (socketSensor)
	{
		const FVector sensorLocation = 
			socketSensor->GetSocketLocation(skeletalMesh);
		const FRotator sensorRot{ 0.f, yaw(), 0.f };
		OutLocation = sensorLocation;
		OutRotation = sensorRot;
	}
	else
	{
		Super::GetActorEyesViewPoint(OutLocation, OutRotation);
	}
}
void ATurret::BeginPlay()
{
	Super::BeginPlay();
	if (pawnSense)
	{
		if (!pawnSense->OnSeePawn.IsAlreadyBound(this, &ATurret::onSeePawn))
		{
			pawnSense->OnSeePawn.AddDynamic(this, &ATurret::onSeePawn);
		}
	}
}
void ATurret::onSeePawn(APawn* pawn)
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
		FString::Printf(TEXT("I SEE U!")));
}
