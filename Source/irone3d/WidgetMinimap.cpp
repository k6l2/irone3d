// YOLO SWAG 420
#include "WidgetMinimap.h"
#include "LevelMap.h"
#include "WidgetMinimapSquare.h"
#include <Runtime/UMG/Public/Components/UniformGridPanel.h>
#include <Runtime/UMG/Public/Components/UniformGridSlot.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
UWidgetMinimap::UWidgetMinimap(const FObjectInitializer & ObjectInitializer)
	: Super(ObjectInitializer)
	, gridPanel(nullptr)
{
}
void UWidgetMinimap::updateMinimap(const ULevelMap* const levelMap)
{
	// iterate over each WidgetMinimapSquare
	for (uint8 y = 0; y < ULevelMap::ROOM_ARRAY_SIZE; y++)
	{
		for (uint8 x = 0; x < ULevelMap::ROOM_ARRAY_SIZE; x++)
		{
			const ULevelMap::RoomCoord gridCoord{ x,y };
			const uint16 gridIndex = y * ULevelMap::ROOM_ARRAY_SIZE + x;
			UWidgetMinimapSquare* square = gridSquares[gridIndex];
			//	clear the player location sprite
			const bool isVisited		= levelMap->isVisited(gridCoord);
			const bool isPlayerLocation = gridCoord == levelMap->getCurrCoord();
			const bool hasNorthExit = levelMap->hasExitNorth(gridCoord);
			const bool hasSouthExit = levelMap->hasExitSouth(gridCoord);
			const bool hasWestExit  = levelMap->hasExitWest (gridCoord);
			const bool hasEastExit  = levelMap->hasExitEast (gridCoord);
			square->setTileProperties(isVisited, isPlayerLocation,
				hasNorthExit, hasEastExit, hasSouthExit, hasWestExit);
		}
	}
}
void UWidgetMinimap::setGridPanel(UUniformGridPanel* gp)
{
	///APlayerController* pc = GetOwningPlayer();
	UWorld* const world = GetWorld();
	///GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
	///	FString::Printf(TEXT("BOOP?")));
	if (!gridPanel && world)
	{
		gridSquares.SetNum(0);
		///GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		///	FString::Printf(TEXT("BEEP!")));
		// when we first set the gridPanel, we need to populate it with
		//	MinimapSquare widgets to fill a ULevelMap::ROOM_ARRAY_SIZE^2 grid
		for (uint8 y = 0; y < ULevelMap::ROOM_ARRAY_SIZE; y++)
		{
			for (uint8 x = 0; x < ULevelMap::ROOM_ARRAY_SIZE; x++)
			{
				UWidgetMinimapSquare* square =
					CreateWidget<UWidgetMinimapSquare>(
						world, minimapSquareWidgetClass);
				check(square);
				UUniformGridSlot* slot = gp->AddChildToUniformGrid(square);
				check(slot);
				slot->SetRow   (y);
				slot->SetColumn(x);
				slot->SetHorizontalAlignment(
					EHorizontalAlignment::HAlign_Fill);
				slot->SetVerticalAlignment(
					EVerticalAlignment::VAlign_Fill);
				gridSquares.Push(square);
			}
		}
	}
	gridPanel = gp;
}
