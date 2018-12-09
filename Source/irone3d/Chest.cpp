// YOLO SWAG 420
#include "Chest.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include "Irone3dGameState.h"
#include "LevelMap.h"
AChest::AChest()
	:meshComponent(CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("ChestMesh")))
{
	RootComponent = meshComponent;
	PrimaryActorTick.bCanEverTick = false;
}
void AChest::BeginPlay()
{
	Super::BeginPlay();
	UWorld*const world = GetWorld();
	if (world)
	{
		if (randomLoot)
		{
			AIrone3dGameState const*const gs =
				world->GetGameState<AIrone3dGameState>();
			if (gs)
			{
				ULevelMap *const levelMap = gs->getLevelMap();
				if (levelMap)
				{
					auto setRandomNonKeyLoot = [&]()->void
					{
						switch (FMath::RandRange(0, 2))
						{
						case 0:
							loot.type = ItemType::HEART;
							break;
						case 1:
							loot.type = ItemType::BOOT;
							break;
						case 2:
							loot.type = ItemType::SWORD;
							break;
						}
					};
					if (levelMap->getHasSpawnedKeyYet())
					{
						setRandomNonKeyLoot();
					}
					else
					{
						const uint16 totalRooms = 
							levelMap->getTotalNormalRooms();
						const uint16 unexploredRooms = 
							levelMap->getTotalNormalUnexploredRooms();
						UE_LOG(LogTemp, Warning, 
							TEXT("totalRooms=%i unexplored=%i"),
							totalRooms, unexploredRooms);
						// if we have no more unexplored rooms left in the 
						//	level, the chance of this chest dropping a key is
						//	100%
						const float percentKeyChance = unexploredRooms > 0 ?
							1.f / totalRooms : 1.f;
						if (FMath::FRand() <= percentKeyChance)
						{
							loot.type = ItemType::KEY;
							levelMap->setHasSpawnedKeyYet(true);
						}
						else
						{
							setRandomNonKeyLoot();
						}
					}
				}
			}
		}
		AItem* newItem = world->SpawnActor<AItem>(bpItemClass.Get(),
			GetActorLocation(),GetActorRotation());
		newItem->setMeta(loot);
	}
	Destroy();
}
