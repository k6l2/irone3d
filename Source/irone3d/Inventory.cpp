// YOLO SWAG 420
#include "Inventory.h"
uint32 UInventory::getItemCount(ItemType iType) const
{
	if (itemCounts.Contains(iType))
	{
		return itemCounts[iType];
	}
	return 0;
}
void UInventory::addItem(ItemType iType)
{
	itemCounts[iType] = getItemCount(iType) + 1;
}
