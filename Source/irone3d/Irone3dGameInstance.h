// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Irone3dGameInstance.generated.h"
UCLASS()
class IRONE3D_API UIrone3dGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UFUNCTION()
		uint8 getNextFloorNumber() const;
	UFUNCTION()
		void setNextFloorNumber(uint8 value);
private:
	UPROPERTY()
		uint8 nextFloorNumber = 0;
};
