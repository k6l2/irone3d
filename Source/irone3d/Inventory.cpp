// YOLO SWAG 420
#include "Inventory.h"
#include "irone3dGameMode.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Irone3DPlayer.h"
int32 UInventory::getItemCount(ItemType iType) const
{
	if (itemCounts.Contains(iType))
	{
		/// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
		/// 	FString::Printf(TEXT("itemCounts[%i]=%i"),
		/// 		int(iType), itemCounts[iType]));
		return itemCounts[iType];
	}
	return 0;
}
void UInventory::addItem(ItemType iType)
{
	if (itemCounts.Contains(iType))
	{
		itemCounts.Add(iType, itemCounts[iType] + 1);
	}
	else
	{
		itemCounts.Add(iType, 1);
	}
	/// GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Yellow,
	/// 	FString::Printf(TEXT("new item count=%i"), getItemCount(iType)));
	///itemCounts[iType] = getItemCount(iType) + 1;
	UWorld const*const world = GetWorld();
	ensure(world);
	Airone3dGameMode*const gm = world->GetAuthGameMode<Airone3dGameMode>();
	ensure(gm);
	gm->updateInventoryWidgets(this);
}
bool UInventory::removeItem(ItemType iType)
{
	if (!itemCounts.Contains(iType))
	{
		return false;
	}
	itemCounts.Add(iType, itemCounts[iType] - 1);
	if (itemCounts[iType] <= 0)
	{
		itemCounts.Remove(iType);
	}
	UWorld const*const world = GetWorld();
	ensure(world);
	Airone3dGameMode*const gm = world->GetAuthGameMode<Airone3dGameMode>();
	ensure(gm);
	gm->updateInventoryWidgets(this);
	return true;
}
TMap<ItemType, uint32>const& UInventory::getItemCounts() const
{
	return itemCounts;
}
void UInventory::setItemCounts(TMap<ItemType, uint32>const& counts)
{
	itemCounts = counts;
	UWorld const*const world = GetWorld();
	ensure(world);
	Airone3dGameMode*const gm = world->GetAuthGameMode<Airone3dGameMode>();
	ensure(gm);
	gm->updateInventoryWidgets(this);
}
