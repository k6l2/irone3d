// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Item.h"
#include "Inventory.generated.h"
UCLASS()
class IRONE3D_API UInventory : public UObject
{
	GENERATED_BODY()
public:
	uint32 getItemCount(ItemType iType) const;
	void addItem(ItemType iType);
private:
	UPROPERTY(VisibleAnywhere, Category = Inventory)
		TMap<ItemType, uint32> itemCounts;
};
