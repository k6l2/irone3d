// YOLO SWAG 420
#include "Boss.h"
#include "irone3d.h"
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/GameFramework/PlayerController.h>
ABoss::ABoss()
	: componentSphere(CreateDefaultSubobject<USphereComponent>(TEXT("sphere")))
	, componentMesh(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("mesh")))
{
	RootComponent = componentSphere;
	componentMesh->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}
void ABoss::BeginPlay()
{
	Super::BeginPlay();
	auto* const world = GetWorld();
	if (!world)
	{
		return;
	}
	static const float TRACE_SIZE = 100000;
	FHitResult hitResult;
	FCollisionQueryParams queryParams;
	// find min X
	{
		const FVector traceStart = GetActorLocation();
		const FVector traceEnd   = traceStart - FVector{ TRACE_SIZE,0,0 };
		const bool hitSomething  = world->LineTraceSingleByChannel(
			hitResult, traceStart, traceEnd,
			ECC_EnemyVision, queryParams);
		if (hitSomething)
		{
			patrolAreaMin.X = hitResult.Location.X;
		}
		else
		{
			patrolAreaMin.X = -TRACE_SIZE;
		}
	}
	// find min Y
	{
		const FVector traceStart = GetActorLocation();
		const FVector traceEnd   = traceStart - FVector{ 0,TRACE_SIZE,0 };
		const bool hitSomething  = world->LineTraceSingleByChannel(
			hitResult, traceStart, traceEnd,
			ECC_EnemyVision, queryParams);
		if (hitSomething)
		{
			patrolAreaMin.Y = hitResult.Location.Y;
		}
		else
		{
			patrolAreaMin.Y = -TRACE_SIZE;
		}
	}
	// find max X
	{
		const FVector traceStart = GetActorLocation();
		const FVector traceEnd   = traceStart + FVector{ TRACE_SIZE,0,0 };
		const bool hitSomething  = world->LineTraceSingleByChannel(
			hitResult, traceStart, traceEnd,
			ECC_EnemyVision, queryParams);
		if (hitSomething)
		{
			patrolAreaMax.X = hitResult.Location.X;
		}
		else
		{
			patrolAreaMax.X = TRACE_SIZE;
		}
	}
	// find max Y
	{
		const FVector traceStart = GetActorLocation();
		const FVector traceEnd   = traceStart + FVector{ 0,TRACE_SIZE,0 };
		const bool hitSomething  = world->LineTraceSingleByChannel(
			hitResult, traceStart, traceEnd,
			ECC_EnemyVision, queryParams);
		if (hitSomething)
		{
			patrolAreaMax.Y = hitResult.Location.Y;
		}
		else
		{
			patrolAreaMax.Y = TRACE_SIZE;
		}
	}
	// account for the size of our hitsphere
	patrolAreaMin.X += componentSphere->GetScaledSphereRadius();
	patrolAreaMin.Y += componentSphere->GetScaledSphereRadius();
	patrolAreaMax.X -= componentSphere->GetScaledSphereRadius();
	patrolAreaMax.Y -= componentSphere->GetScaledSphereRadius();
	// set the Z axis bindings
	patrolAreaMin.Z = GetActorLocation().Z;
	patrolAreaMax.Z = GetActorLocation().Z;
	// initialize patrol position
	patrolDestination = { GetActorLocation().X, GetActorLocation().Y, 0 };
	prevLocation = GetActorLocation();
}
void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// obtain the target player //
	const UWorld* const world = GetWorld();
	FConstPlayerControllerIterator controllerIt =
		world->GetPlayerControllerIterator();
	for (; controllerIt; ++controllerIt)
	{
		auto weakController = *controllerIt;
		if (!weakController.IsValid())
		{
			continue;
		}
		auto controller = weakController.Get();
		auto pawn = controller->GetPawn();
		if (pawn)
		{
			targetPawn = pawn;
			break;
		}
	}
	// hover along the Z axis //
	static const float HOVER_ACCEL = 500.f;
	static const float HOVER_MAX_SPEED = 100.f;
	if (((hoverUp  && GetActorLocation().Z > patrolAreaMax.Z) ||
		 (!hoverUp && GetActorLocation().Z < patrolAreaMin.Z))&&
		FMath::Abs(velocity.Z) >= HOVER_MAX_SPEED)
	{
		hoverUp = !hoverUp;
	}
	const float zFloatDir = hoverUp ? 1.f : -1.f;
	velocity.Z += zFloatDir * HOVER_ACCEL*DeltaTime;
	velocity.Z = FMath::Clamp(velocity.Z, -HOVER_MAX_SPEED, HOVER_MAX_SPEED);
	// rotate yaw so we're looking at the target pawn //
	auto lookAtVector2D = [&](const FVector& dir)
	{
		const float radiansBetween = FMath::Acos(
			FVector::DotProduct(GetActorForwardVector(), dir));
		const FVector crossProd =
			FVector::CrossProduct(GetActorForwardVector(), dir);
		const FVector newFacingVec = GetActorForwardVector().RotateAngleAxis(
			FMath::RadiansToDegrees(radiansBetween*DeltaTime), crossProd);
		SetActorRotation(newFacingVec.Rotation());
	};
	const FVector location2D(GetActorLocation().X, GetActorLocation().Y, 0);
	const FVector toPatrolLoc = patrolDestination - location2D;
	FVector toPatrolLocNorm;
	float   toPatrolLocMag;
	toPatrolLoc.ToDirectionAndLength(toPatrolLocNorm, toPatrolLocMag);
	{
		FVector toTargetVec2D(0, -1, 0);
		if (toPatrolLocMag > 10)
		{
			toTargetVec2D = toPatrolLocNorm;
		}
		else if (auto tPawn = targetPawn.Get())
		{
			toTargetVec2D = tPawn->GetActorLocation() - GetActorLocation();
			toTargetVec2D.Z = 0;
			toTargetVec2D = toTargetVec2D.GetSafeNormal();
		}
		lookAtVector2D(toTargetVec2D);
	}
	// move to the next patrol location //
	if (toPatrolLocMag > 0)
	{
		static const float MAX_SPEED = 1000;
		static const float ACCEL = 1000;
		static const float DECEL_CM = 500;
		velocity += ACCEL * DeltaTime* toPatrolLocNorm;
		const float throttle = FMath::Clamp(
			toPatrolLocMag / DECEL_CM, 0.2f, 1.f);
		const float maxSpeed = MAX_SPEED * throttle;
		FVector velocity2d(velocity.X, velocity.Y, 0);
		if (velocity2d.Size() > maxSpeed)
		{
			velocity2d = velocity2d.GetSafeNormal()*maxSpeed;
			velocity.X = velocity2d.X;
			velocity.Y = velocity2d.Y;
		}
		FVector prevLocation2D(prevLocation.X, prevLocation.Y, 0);
		if (FVector::DotProduct(prevLocation2D - patrolDestination,
			location2D - patrolDestination) < 0)
		{
			// we have reached the patrol destination, stopping condition here
			velocity.X = velocity.Y = 0;
			auto loc = GetActorLocation();
			loc.X = patrolDestination.X;
			loc.Y = patrolDestination.Y;
			SetActorLocation(loc);
		}
	}
	// AI //
	if (idleTime <= 0 && chargeOrbTime <= 0 && orbAttackCooldownTime <= 0)
	{
		// reset AI timers when they are all depleted
		idleTime = 3;
		chargeOrbTime = 1;
		orbAttackCooldownTime = 3;
	}
	if (idleTime > 0)
	{
		idleTime -= DeltaTime;
		if (idleTime <= 0)
		{
			// choose a new patrol location //
			// randomly choose a new location and go there //
			patrolDestination.X = FMath::FRandRange(
				patrolAreaMin.X, patrolAreaMax.X);
			patrolDestination.Y = FMath::FRandRange(
				patrolAreaMin.Y, patrolAreaMax.Y);
		}
	}
	else if (chargeOrbTime > 0)
	{
		chargeOrbTime -= DeltaTime;
		if (chargeOrbTime <= 0)
		{
			// perform orb launch attack //
		}
	}
	else if (orbAttackCooldownTime > 0)
	{
		orbAttackCooldownTime -= DeltaTime;
	}
	// update actor location due to velocity //
	prevLocation = GetActorLocation();
	SetActorLocation(prevLocation + velocity*DeltaTime, true);
}
void ABoss::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
