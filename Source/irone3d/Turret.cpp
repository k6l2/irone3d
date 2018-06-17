// YOLO SWAG 420
#include "Turret.h"
#include "PovPawnSensingComponent.h"
#include "Irone3DPlayer.h"
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Engine/SkeletalMeshSocket.h>
///#include <Runtime/AIModule/Classes/Perception/PawnSensingComponent.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <DrawDebugHelpers.h>
ATurret::ATurret()
	:skeletalMesh(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skeletalMesh")))
	, pawnSense  (CreateDefaultSubobject<UPovPawnSensingComponent> (TEXT("pawnSense")))
	, pawnTarget(nullptr)
	, yawVector(1.f, 0.f)
	, animationActive(false)
	, animationInactive(false)
{
	PrimaryActorTick.bCanEverTick = true;
}
void ATurret::Tick(float deltaSeconds)
{
	const auto* const world = GetWorld();
	if (!world)
	{
		return;
	}
	if (active())
	{
		FVector  eyesLocation;
		FRotator eyesRotation;
		GetActorEyesViewPoint(eyesLocation, eyesRotation);
		FHitResult hitResult;
		FCollisionQueryParams queryParams;
		const FVector traceStart = eyesLocation + GetActorUpVector()*25.f;
		const FVector traceEnd   = pawnTarget->GetActorLocation();
		const bool hitSomething = world->LineTraceSingleByProfile(
			hitResult, traceStart, traceEnd,
			"IgnoreOnlyEnemy", queryParams);
		if (hitSomething)
		{
			const AActor* const hitActor = hitResult.Actor.Get();
			if (!hitActor || hitActor != pawnTarget)
			{
				///GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
				///	FString::Printf(TEXT("hitActor=\"%s\""), *hitActor->GetName()));
				deactivate();
			}
			DrawDebugLine(world, traceStart, traceEnd, FColor::Red);
		}
		else
		{
			///DrawDebugLine(world, traceStart, traceEnd, FColor::Blue);
			deactivate();
		}
		if (pawnTarget)
		{
			///TODO: rotate yaw & pitch to aim at the target!
		}
	}
	else if(animationInactive)
	{
		static const float YAW_PATROL_ROTATION_RATE = 100.f;
		yawVector = yawVector.GetRotated(
			YAW_PATROL_ROTATION_RATE*deltaSeconds);
	}
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
	return pawnTarget != nullptr;
}
void ATurret::setAnimationActive(bool state)
{
	animationActive = state;
}
void ATurret::setAnimationInactive(bool state)
{
	animationInactive = state;
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
	///GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
	///	FString::Printf(TEXT("I SEE U!")));
	pawnTarget = pawn;
}
void ATurret::deactivate()
{
	pawnTarget = nullptr;
}
