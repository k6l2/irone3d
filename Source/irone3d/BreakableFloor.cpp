// YOLO SWAG 420
#include "BreakableFloor.h"
ABreakableFloor::ABreakableFloor()
{
	PrimaryActorTick.bCanEverTick = false;
}
void ABreakableFloor::BeginPlay()
{
	Super::BeginPlay();
}
void ABreakableFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
