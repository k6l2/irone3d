// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Item.h"
#include "Irone3dGameInstance.generated.h"
UCLASS()
class IRONE3D_API UIrone3dGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UIrone3dGameInstance(const FObjectInitializer& ObjectInitializer);
	UFUNCTION()
		uint8 getNextFloorNumber() const;
	UFUNCTION()
		void setNextFloorNumber(uint8 value);
	UFUNCTION()
		TMap<ItemType, uint32>const& getItemCounts() const;
	UFUNCTION()
		void setItemCounts(TMap<ItemType, uint32>const& counts);
private:
	UPROPERTY()
		uint8 nextFloorNumber = 0;
	UPROPERTY()
		TMap<ItemType, uint32> itemCounts;
};
