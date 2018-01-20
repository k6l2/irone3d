// YOLO SWAG 420
#include "Irone3dGameState.h"
#include "LevelMap.h"
AIrone3dGameState::AIrone3dGameState()
	:levelMap(CreateDefaultSubobject<ULevelMap>(TEXT("levelMap")))
{
}
void AIrone3dGameState::generateLevelMap(UWorld* world)
{
	int8 floorNumber = 0;///TODO: actually have this as a variable that
	///increases between floors...
	levelMap->generateNewLevel(world, floorNumber);
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
