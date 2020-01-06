// YOLO SWAG 420
#include "Irone3dGameInstance.h"
UIrone3dGameInstance::UIrone3dGameInstance(
	const FObjectInitializer& ObjectInitializer)
{
	// default starting player inventory (including HP)
	itemCounts.Add(ItemType::HEART, 5);
}
uint8 UIrone3dGameInstance::getNextFloorNumber() const
{
	return nextFloorNumber;
}
void UIrone3dGameInstance::setNextFloorNumber(uint8 value)
{
	nextFloorNumber = value;
}
TMap<ItemType, uint32>const& UIrone3dGameInstance::getItemCounts() const
{
	return itemCounts;
}
void UIrone3dGameInstance::setItemCounts(TMap<ItemType, uint32>const& counts)
{
	itemCounts = counts;
}
