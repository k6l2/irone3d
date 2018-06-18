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
			///DrawDebugLine(world, traceStart, traceEnd, FColor::Red);
		}
		else
		{
			///DrawDebugLine(world, traceStart, traceEnd, FColor::Blue);
			deactivate();
		}
		if (pawnTarget)
		{
			static const float AIM_TO_TARGET_LERP_ALPHA = 0.1f;
			const FVector toTargetVector = traceEnd - traceStart;
			/// const FVector toTargetVector = 
			/// 	GetActorRotation().GetInverse().RotateVector(
			/// 		traceEnd - traceStart);
			/// DrawDebugLine(world, traceStart, traceStart + toTargetVector, FColor::Yellow);
			/// const FTransform actorTransform = GetActorTransform();
			/// FVector toTargetVector = 
			/// 	actorTransform.TransformPosition(
			/// 		traceEnd - traceStart);
			FRotator newAimRot = FMath::Lerp(
				aimVector.Rotation(), toTargetVector.Rotation(),
				AIM_TO_TARGET_LERP_ALPHA);
			aimVector = newAimRot.Vector();
		}
	}
	else if(animationInactive)
	{
		static const float YAW_PATROL_ROTATION_RATE = 100.f;
		aimVector = aimVector.RotateAngleAxis(
			YAW_PATROL_ROTATION_RATE*deltaSeconds,
			GetActorUpVector());
		/// DEBUG ///////////////////
		/// FVector  eyesLocation;
		/// FRotator eyesRotation;
		/// GetActorEyesViewPoint(eyesLocation, eyesRotation);
		/// const FVector traceStart   = eyesLocation + GetActorUpVector()*25.f;
		/// const FVector traceForward = traceStart + GetActorForwardVector()*100.f;
		/// const FVector traceUp      = traceStart + GetActorUpVector()*100.f;
		/// DrawDebugLine(world, traceStart, traceForward, FColor::Red);
		/// DrawDebugLine(world, traceStart, traceUp, FColor::Blue);
	}
}
float ATurret::yaw() const
{
	// calculate the "right" vector using forwardVector & upVector //
	const FVector forwardVector = GetActorForwardVector();
	const FVector upVector = GetActorUpVector();
	const FVector rightVector = FVector::CrossProduct(
		forwardVector, upVector);
	// calculate the aimUpComponent by projecting aim onto upVector //
	const FVector aimUpComp = aimVector.ProjectOnTo(upVector);
	// calculate yawVector by subtracting the upComp from aimVec //
	const FVector yawVector = aimVector - aimUpComp;
	// calculate |yaw| by finding the angle between yawVector and forwardVector //
	const float dotProd = FVector::DotProduct(
		yawVector.GetSafeNormal(), forwardVector);
	///TODO: handle this edge case (aimVector pointing straight up or down)
	float yawDegrees = FMath::Acos(dotProd) * 180 / PI;
	// calculate yaw by using forwardVector cross yawVector //
	const FVector forwardCrossYaw = FVector::CrossProduct(
		forwardVector, yawVector);
	const float dotUpVec = FVector::DotProduct(
		forwardCrossYaw, upVector);
	return yawDegrees * (dotUpVec > 0 ? 1.f : -1.f);
}
float ATurret::pitch() const
{
	// calculate the "right" vector using aimForwardVector & upVector //
	const FVector aimForwardVector = 
		GetActorRotation().RotateVector(
			FRotator{ 0.f, yaw(), 0.f }.RotateVector(
				GetActorForwardVector()));
	const FVector rightVector = FVector::CrossProduct(
		aimForwardVector, GetActorUpVector());
	/// DEBUG ///////////////////
	/// const auto* const world = GetWorld();
	/// FVector  eyesLocation;
	/// FRotator eyesRotation;
	/// GetActorEyesViewPoint(eyesLocation, eyesRotation);
	/// const FVector traceStart = eyesLocation + GetActorUpVector()*25.f;
	/// const FVector traceForward = traceStart + forwardVector *100.f;
	/// const FVector traceUp = traceStart + GetActorUpVector()*100.f;
	/// DrawDebugLine(world, traceStart, traceForward, FColor::Red);
	/// /////////////////////////////
	// calculate aimPerp by projecting aimVector onto the rightVector //
	const FVector aimPerp = aimVector.ProjectOnTo(rightVector);
	// calculate pitchVector by subtracting aimPerp from aimVector //
	const FVector pitchVector = aimVector - aimPerp;
	///TODO: handle edge case where pitchVector is 0,0,0
	// calculate |pitch| by measuring the angle between pitchVector and aimForwardVector //
	const float dotProd = FVector::DotProduct(
		pitchVector.GetSafeNormal(), aimForwardVector);
	float pitchDegrees = FMath::Acos(dotProd) * 180 / PI;
	// calculate pitch by using aimForwardVector cross pitchVector //
	const FVector forwardCrossPitch = FVector::CrossProduct(
		aimForwardVector, pitchVector);
	const float dotRightVec = FVector::DotProduct(
		forwardCrossPitch, rightVector);
	return pitchDegrees * (dotRightVec > 0 ? 1.f : -1.f);
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
		///TODO: figure out why sensor rot always appears on the X/Y plane?!? (at least for the sensor component it does)
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
	aimVector = GetActorForwardVector();
	/// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red,
	/// 	FString::Printf(TEXT("aimVector=%s"), *aimVector.ToString()));
	/// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green,
	/// 	FString::Printf(TEXT("upVector=%s"), *GetActorUpVector().ToString()));
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
