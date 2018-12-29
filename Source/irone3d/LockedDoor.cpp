// YOLO SWAG 420
#include "LockedDoor.h"
#include "Irone3dGameState.h"
#include "Engine/World.h"
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystemComponent.h>
ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;
}
void ALockedDoor::unlock()
{
	UWorld const*const world = GetWorld();
	if (ensure(world))
	{
		UGameplayStatics::PlaySoundAtLocation(world, sfxDestroyed,
			GetActorLocation());
		auto particleComp =
			UGameplayStatics::SpawnEmitterAtLocation(world,
				particleSystemUnlock, GetActorLocation());
		//particleComp->SetWorldScale3D(FVector(10, 10, 10));
	}
	Destroy();
}
void ALockedDoor::BeginPlay()
{
	Super::BeginPlay();
	UWorld const*const world = GetWorld();
	ensure(world);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	gs->addActorToCurrentRoom(this);
}
///void ALockedDoor::Tick(float DeltaTime)
///{
///	Super::Tick(DeltaTime);
///}
