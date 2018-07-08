// YOLO SWAG 420
#include "Inventory.h"
#include "irone3dGameMode.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
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
	UWorld* world = GetWorld();
	check(world);
	if (!world)
	{
		return;
	}
	Airone3dGameMode* gm = world->GetAuthGameMode<Airone3dGameMode>();
	check(gm);
	if (!gm)
	{
		return;
	}
	gm->updateInventoryWidgets(this);
}
