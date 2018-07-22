// YOLO SWAG 420
#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelMap.generated.h"
/*
	This class should handle the generation of Irone's logical map
		which is just a 2D array of Level instances.
	This class should also be able to handle the 
		procedural generation of such a logical map
	It should also be capable of managing the meta for the minimap:
		that is, which rooms the player has visited and which have not
	The only instance of this should be handled by the GameState!
 */
USTRUCT()
struct FLevelGenNode
{
	GENERATED_BODY();
public:
	FLevelGenNode();
	uint8 exitCount() const;
	bool hasOneExit() const;
public:
	bool hasNorth;
	bool hasSouth;
	bool hasEast;
	bool hasWest;
	bool hasBeenVisited;
	bool bossRoom;
	FString uniqueLevelName;
	// AActors need to add themselves to this set of AActors manually
	//	when they are spawned into the world, most likely via GameState
	UPROPERTY()
		TArray<TWeakObjectPtr<AActor>> actorsOwnedByThisRoom;
};
UCLASS(BlueprintType)
class IRONE3D_API ULevelMap : public UObject
{
	GENERATED_BODY()
public:
	static const uint8 ROOM_ARRAY_SIZE;
	class RoomCoord
	{
	public:
		RoomCoord(uint8 x = ROOM_ARRAY_SIZE, uint8 y = ROOM_ARRAY_SIZE);
		uint8 x() const;
		uint8 y() const;
		bool operator==(const RoomCoord& other) const;
	private:
		uint8 m_x;
		uint8 m_y;
	};
private:
	static const float ROOM_SIZE;
	struct LevelGenEdge
	{
		// Even though these names suggest edge directionality,
		//	this algorithm treats these edges as bi-directional.
		int32 indexFrom;
		int32 indexTo;
		float weight;
	};
public:
	void generateNewLevel(UWorld* world, int8 floorNumber);
	RoomCoord getStartCoord() const;
	RoomCoord getCurrCoord() const;
	// sets the "hasBeenVisited" flag in the new room to true
	//	returns true if the room we move to was previously not visited
	bool advanceCurrCoord(const FVector& exitVec);
	FString currentRoomLevelName();
	FString adjacentRoomLevelName(const FVector& exitVec);
	void addActorToCurrentRoom(AActor* actor);
	// returns a copy of the current room's actor set should only be called
	//	like 1-2 times during a transition, so the performance of the 
	//	copy/nullify doesn't really matter all that much.
	TArray<TWeakObjectPtr<AActor>> getCurrentRoomActorSet() const;
	FVector currentRoomWorldOffset() const;
	bool isVisited(const RoomCoord& coord) const;
	bool hasExitNorth(const RoomCoord& coord) const;
	bool hasExitEast (const RoomCoord& coord) const;
	bool hasExitSouth(const RoomCoord& coord) const;
	bool hasExitWest (const RoomCoord& coord) const;
	UFUNCTION(BlueprintCallable, Category = LevelMap)
		int getFloorNumber() const;
private:
	FString findLevelDir(const FLevelGenNode& node);
	void exitVecToOffsets(const FVector& exitVec,
		int8& outOffsetX, int8& outOffsetY) const;
	/// DEBUG TEST FUNCTION to figure out wtf is going on w/ BSP not 
	///		getting a world offset
	UFUNCTION(BlueprintCallable, Category = Transitions)
		void onStartLevelStreamLoaded();
private:
	UPROPERTY(VisibleAnywhere, Category=Transitions)
	TArray<FLevelGenNode> finalLevelLayout;
	RoomCoord startCoord;
	RoomCoord currCoord;
	int8 floorNumber;
};
