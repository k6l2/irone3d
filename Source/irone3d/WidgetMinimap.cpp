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
void UWidgetMinimap::setGridPanel(class UUniformGridPanel* gp)
{
	///APlayerController* pc = GetOwningPlayer();
	UWorld* const world = GetWorld();
	///GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
	///	FString::Printf(TEXT("BOOP?")));
	if (!gridPanel && world)
	{
		///GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		///	FString::Printf(TEXT("BEEP!")));
		// when we first set the gridPanel, we need to populate it with
		//	MinimapSquare widgets to fill a ULevelMap::ROOM_ARRAY_SIZE^2 grid
		for (uint8 x = 0; x < ULevelMap::ROOM_ARRAY_SIZE; x++)
		{
			for (uint8 y = 0; y < ULevelMap::ROOM_ARRAY_SIZE; y++)
			{
				UWidgetMinimapSquare* square =
					CreateWidget<UWidgetMinimapSquare>(
						world, minimapSquareWidgetClass);
				check(square);
				UUniformGridSlot* slot = gp->AddChildToUniformGrid(square);
				check(slot);
				slot->SetRow   (y);
				slot->SetColumn(x);
				///gp->AddChild(square);
				///if (UUniformGridSlot* slot = 
				///		Cast<UUniformGridSlot>(square->Slot))
				///{
				///	slot->SetRow   (y);
				///	slot->SetColumn(x);
				///}
			}
		}
	}
	gridPanel = gp;
}
