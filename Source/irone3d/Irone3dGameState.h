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
	void generateLevelMap(UWorld* world);
	FString currentRoomLevelName();
	FString adjacentRoomLevelName(const FVector& exitVec);
	void addActorToCurrentRoom(AActor* actor);
	TArray<TWeakObjectPtr<AActor>> getCurrentRoomActorSet() const;
	bool advanceCurrCoord(const FVector& exitVec);
	FVector currentRoomWorldOffset() const;
	class ULevelMap* getLevelMap() const;
private:
	UPROPERTY()
		class ULevelMap* levelMap;
	UPROPERTY()
		class UInventory* inventory;
};
