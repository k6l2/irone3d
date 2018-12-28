// YOLO SWAG 420
#include "LockedDoor.h"
#include "Irone3dGameState.h"
#include "Engine/World.h"
ALockedDoor::ALockedDoor()
{
	PrimaryActorTick.bCanEverTick = false;
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
