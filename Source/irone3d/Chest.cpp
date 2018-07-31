// YOLO SWAG 420
#include "Chest.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
AChest::AChest():
	meshComponent(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh")))
{
	RootComponent = meshComponent;
	PrimaryActorTick.bCanEverTick = false;
}
void AChest::BeginPlay()
{
	Super::BeginPlay();
	UWorld* world = GetWorld();
	if (world)
	{
		AItem* newItem = world->SpawnActor<AItem>(bpItemClass.Get(),GetActorLocation(),GetActorRotation());
		newItem->setMeta(loot);
		//world->SpawnActor<AItem>();
	}

	Destroy();
}
