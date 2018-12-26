// YOLO SWAG 420
#include "Irone3dGameState.h"
#include "LevelMap.h"
#include "Inventory.h"
#include "Irone3dGameInstance.h"
AIrone3dGameState::AIrone3dGameState()
	: levelMap (CreateDefaultSubobject<ULevelMap >(TEXT("levelMap")))
	, inventory(CreateDefaultSubobject<UInventory>(TEXT("inventory")))
{
}
void AIrone3dGameState::generateLevelMap(UWorld* world, bool debugRoomLoaded)
{
	UIrone3dGameInstance*const gi = 
		Cast<UIrone3dGameInstance>(GetGameInstance());
	if (gi)
	{
		nextFloorNumber = gi->getNextFloorNumber();
		levelMap->generateNewLevel(world, nextFloorNumber, debugRoomLoaded);
		nextFloorNumber++;
		gi->setNextFloorNumber(nextFloorNumber);
	}
}
FString AIrone3dGameState::currentRoomLevelName()
{
	return levelMap->currentRoomLevelName();
}
FString AIrone3dGameState::adjacentRoomLevelName(const FVector & exitVec)
{
	return levelMap->adjacentRoomLevelName(exitVec);
}
void AIrone3dGameState::addActorToCurrentRoom(AActor * actor)
{
	levelMap->addActorToCurrentRoom(actor);
}
TArray<TWeakObjectPtr<AActor>> AIrone3dGameState::getCurrentRoomActorSet() const
{
	return levelMap->getCurrentRoomActorSet();
}
bool AIrone3dGameState::advanceCurrCoord(const FVector & exitVec)
{
	return levelMap->advanceCurrCoord(exitVec);
}
FVector AIrone3dGameState::currentRoomWorldOffset() const
{
	return levelMap->currentRoomWorldOffset();
}
class ULevelMap* AIrone3dGameState::getLevelMap() const
{
	return levelMap;
}
class UInventory* AIrone3dGameState::getInventory() const
{
	return inventory;
}
