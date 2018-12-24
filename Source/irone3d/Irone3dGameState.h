// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Irone3dGameState.generated.h"
UCLASS()
class IRONE3D_API AIrone3dGameState : public AGameStateBase
{
	GENERATED_BODY()
public:
	AIrone3dGameState();
	void generateLevelMap(UWorld* world, bool debugRoomLoaded);
	FString currentRoomLevelName();
	FString adjacentRoomLevelName(const FVector& exitVec);
	void addActorToCurrentRoom(AActor* actor);
	TArray<TWeakObjectPtr<AActor>> getCurrentRoomActorSet() const;
	bool advanceCurrCoord(const FVector& exitVec);
	FVector currentRoomWorldOffset() const;
	class ULevelMap* getLevelMap() const;
	class UInventory* getInventory() const;
private:
	UPROPERTY(VisibleAnywhere, Category = IroneGameInstance)
		uint8 nextFloorNumber = 0;
	UPROPERTY()
		class ULevelMap* levelMap;
	UPROPERTY()
		//Note: Do not put this in GameState... This should be in a custom
		//	PlayerState class
		class UInventory* inventory;
};
