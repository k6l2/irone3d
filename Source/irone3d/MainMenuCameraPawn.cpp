// YOLO SWAG 420
#include "MainMenuCameraPawn.h"
#include <Camera/CameraComponent.h>
#include "Irone3DPlayer.h"
#include <Engine/TargetPoint.h>
AMainMenuCameraPawn::AMainMenuCameraPawn()
	:componentCamera(CreateDefaultSubobject<UCameraComponent>(TEXT("camera")))
{
	RootComponent = componentCamera;
	PrimaryActorTick.bCanEverTick = true;
}
AIrone3DPlayer* AMainMenuCameraPawn::getPlayerActor() const
{
	return playerActor;
}
void AMainMenuCameraPawn::yawToPlayerActor()
{
	if (!ensure(playerActor)) return;
	FRotator rotation = GetActorRotation();
	const FVector toPlayerActor = 
		playerActor->GetActorLocation() - GetActorLocation();
	rotation.Yaw = toPlayerActor.Rotation().Yaw;
	SetActorRotation(rotation);
}
void AMainMenuCameraPawn::moveToPanTarget1()
{
	if (!ensure(panTarget1)) return;
	currTargetPoint = panTarget1;
}
void AMainMenuCameraPawn::moveToPanTarget2()
{
	if (!ensure(panTarget2)) return;
	currTargetPoint = panTarget2;
}
void AMainMenuCameraPawn::pitchStraightDown(float deltaSeconds)
{
	FRotator rotation = GetActorRotation();
	const float toDesiredPitch = (-90 - rotation.Pitch);
	const float toDesiredPitchMag = FMath::Abs(toDesiredPitch);
	const float toDesiredPitchDir = toDesiredPitchMag / toDesiredPitch;
	const float pitchThrottle = 
		FMath::Clamp(toDesiredPitchMag / pitchDecelDistance, 0.f, 1.f);
	rotation.Pitch += pitchThrottle * pitchSpeedMax * toDesiredPitchDir * deltaSeconds;
	SetActorRotation(rotation);
}
void AMainMenuCameraPawn::BeginPlay()
{
	Super::BeginPlay();
}
void AMainMenuCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (currTargetPoint)
	{
		const FVector toTarget = 
			currTargetPoint->GetActorLocation() - GetActorLocation();
		const FVector toTargetNorm = toTarget.GetSafeNormal();
		const float toTargetMag = toTarget.Size();
		velocity += toTargetAcceleration * toTargetNorm;
		const float throttle = 
			FMath::Clamp(toTargetMag / toTargetDecelDistance, 0.f, 1.f);
		velocity = velocity.GetSafeNormal()*throttle*toTargetSpeedMax;
	}
	else
	{
		velocity = FVector::ZeroVector;
	}
	SetActorLocation(GetActorLocation() + velocity * DeltaTime);
}
///void AMainMenuCameraPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
///{
///	Super::SetupPlayerInputComponent(PlayerInputComponent);
///}
