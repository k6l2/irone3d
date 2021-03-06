// YOLO SWAG 420
#include "LevelMap.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/Engine/LevelStreaming.h>
#include <Runtime/CoreUObject/Public/UObject/UObjectIterator.h>
#include <Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include <set>
#include <tuple>
#include <queue>
#include "irone3dGameMode.h"
#include <Engine/ObjectLibrary.h>
const uint8 ULevelMap::ROOM_ARRAY_SIZE = 5;
const float ULevelMap::ROOM_SIZE = 2500;
ULevelMap::RoomCoord::RoomCoord(uint8 x, uint8 y)
	:m_x(x), m_y(y)
{
}
uint8 ULevelMap::RoomCoord::x() const
{
	return m_x;
}
uint8 ULevelMap::RoomCoord::y() const
{
	return m_y;
}
bool ULevelMap::RoomCoord::operator==(const RoomCoord & other) const
{
	//if (std::tie(m_x, m_y) == std::tie(other.m_x, other.m_y))
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("RoomCoords are equal! this=%d,%d other=%d,%d"),
	//		m_x, m_y, other.m_x, other.m_y);
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("RoomCoords are NOT equal! this=%d,%d other=%d,%d"),
	//		m_x, m_y, other.m_x, other.m_y);
	//}
	return std::tie(m_x, m_y) == std::tie(other.m_x, other.m_y);
}
FLevelGenNode::FLevelGenNode()
	: hasNorth(false)
	, hasSouth(false)
	, hasEast(false)
	, hasWest(false)
	, hasBeenVisited(false)
	, bossRoom(false)
	, uniqueLevelName("UNINITIALIZED-NAME")
{
}
uint8 FLevelGenNode::exitCount() const
{
	uint8 totalExits = 0;
	if (hasNorth) totalExits++;
	if (hasSouth) totalExits++;
	if (hasEast)  totalExits++;
	if (hasWest)  totalExits++;
	return totalExits;
}
bool FLevelGenNode::hasOneExit() const
{
	return exitCount() == 1;
}
void ULevelMap::generateNewLevel(UWorld* world, uint8 floorNumber, 
	bool debugRoomLoaded)
{
	this->floorNumber = floorNumber;
	hasSpawnedKeyYet = false;
	hasSpawnedBossDoorYet = false;
	// this custom game mode or a similar blueprinted class is needed
	//	in order to call the stupid createLevelInstance function
	auto gm = world->GetAuthGameMode<Airone3dGameMode>();
	if (!gm)
	{
		return;
	}
	// Build a list of all edges with randomized weights
	TArray<LevelGenEdge> edgeList;
	for (uint8 y = 0; y < ROOM_ARRAY_SIZE; y++)
	{
		for (uint8 x = 0; x < ROOM_ARRAY_SIZE; x++)
		{
			LevelGenEdge newEdge;
			newEdge.indexFrom = y * ROOM_ARRAY_SIZE + x;
			newEdge.indexTo = y * ROOM_ARRAY_SIZE + (x + 1);
			newEdge.weight = FMath::RandRange(0.f, 1.f);
			if (x + 1 < ROOM_ARRAY_SIZE)
			{
				edgeList.Add(newEdge);
			}
			newEdge.indexTo = (y + 1)*ROOM_ARRAY_SIZE + x;
			newEdge.weight = FMath::RandRange(0.f, 1.f);
			if (y + 1 < ROOM_ARRAY_SIZE)
			{
				edgeList.Add(newEdge);
			}
		}
	}
	/// DEBUG ///
	///for (auto e : edgeList)
	///{
	///	UE_LOG(LogTemp, Warning, TEXT("Edge={from:%d,to:%d,weight:%f}"),
	///		e.indexFrom, e.indexTo, e.weight);
	///}
	/// ////////////////
	// initialize storage container for final level layout data
	//	to be accumulated during the algorithm
	finalLevelLayout.Empty();
	finalLevelLayout.SetNum(ROOM_ARRAY_SIZE*ROOM_ARRAY_SIZE);
	if (debugRoomLoaded)
	{
		return;
	}
	std::set<int32> pickedNodeIndexes;
	// sort the list of edges based on weight
	auto edgeWeightSort = [](const LevelGenEdge& a, const LevelGenEdge& b)->bool
	{
		// "a" goes before "b" if:
		return a.weight < b.weight;
	};
	edgeList.Sort(edgeWeightSort);
	///DEBUG ///
	///FString strEdgeListWeights;
	///for (auto e : edgeList)
	///{
	///	strEdgeListWeights += FString::SanitizeFloat(e.weight) + ",";
	///}
	///UE_LOG(LogTemp, Warning, 
	///	TEXT("Sorted edgeList weights={%s}"), *strEdgeListWeights);
	/// //////////
	// select the edge with the smallest weight
	//	  and add mark the selected edge's nodes as "picked"
	auto pickEdge = [&](TArray<LevelGenEdge>& list, int32 index)->void
	{
		auto edge = list[index];
		//list.RemoveAt(index);
		pickedNodeIndexes.insert(edge.indexFrom);
		pickedNodeIndexes.insert(edge.indexTo);
		const int32 fromY = edge.indexFrom / ROOM_ARRAY_SIZE;
		const int32 fromX = edge.indexFrom % ROOM_ARRAY_SIZE;
		//const int32 fromI = fromX + fromY * ROOM_ARRAY_SIZE;
		const int32 toY = edge.indexTo / ROOM_ARRAY_SIZE;
		const int32 toX = edge.indexTo % ROOM_ARRAY_SIZE;
		//const int32 toI = toX + toY * ROOM_ARRAY_SIZE;
		if (fromY == toY)
		{
			if (fromX < toX)
			{
				finalLevelLayout[edge.indexFrom].hasEast = true;
				finalLevelLayout[edge.indexTo].hasWest = true;
			}
			else
			{
				check(fromX > toX);
				finalLevelLayout[edge.indexFrom].hasWest = true;
				finalLevelLayout[edge.indexTo].hasEast = true;
			}
		}
		else
		{
			check(fromX == toX);
			if (fromY < toY)
			{
				finalLevelLayout[edge.indexFrom].hasSouth = true;
				finalLevelLayout[edge.indexTo].hasNorth = true;
			}
			else
			{
				check(fromY > toY);
				finalLevelLayout[edge.indexFrom].hasNorth = true;
				finalLevelLayout[edge.indexTo].hasSouth = true;
			}
		}
	};
	pickEdge(edgeList, 0);
	// accumulate list of edges that have only one pickedNodeIndex
	TArray<LevelGenEdge> adjacentEdges;
	auto accumulateAdjacentEdges = [&]()->void
	{
		adjacentEdges.Empty();
		for (int32 e = 0; e < edgeList.Num(); e++)
		{
			auto edge = edgeList[e];
			auto indexFromIt = pickedNodeIndexes.find(edge.indexFrom);
			auto indexToIt = pickedNodeIndexes.find(edge.indexTo);
			if ((indexFromIt == pickedNodeIndexes.end() &&
				 indexToIt != pickedNodeIndexes.end()) ||
				(indexFromIt != pickedNodeIndexes.end() &&
				 indexToIt == pickedNodeIndexes.end()))
			{
				adjacentEdges.Add(edge);
			}
		}
	};
	accumulateAdjacentEdges();
	// while the list of adjacent edges is not empty:
	while (adjacentEdges.Num() > 0)
	{
		//	sort the list of adjacent edges
		adjacentEdges.Sort(edgeWeightSort);
		//	select the edge with the smallest weight
		//		and add mark the selected edge's nodes as "picked"
		pickEdge(adjacentEdges, 0);
		//	accumulate a list of edges that have only one pickedNodeIndex
		accumulateAdjacentEdges();
	}
	// at this point, finalLevelLayout contains our minimum spanning tree /////
	//-Iterate over each room and locate the rooms with one exit
	auto locateOneExitRooms = [&]()->TArray<uint16>
	{
		TArray<uint16> retVal;
		for (uint8 y = 0; y < ROOM_ARRAY_SIZE; y++)
		{
			for (uint8 x = 0; x < ROOM_ARRAY_SIZE; x++)
			{
				const uint16 i = x + y * ROOM_ARRAY_SIZE;
				auto& node = finalLevelLayout[i];
				if (node.hasOneExit())
				{
					retVal.Push(i);
				}
			}
		}
		return retVal;
	};
	/// //-Brushfire over the finalLevelLayout starting from the leaf nodes and 
	/// //	accumulate a list of rooms to cull
	/// std::set<uint16> visitedRoomIndexes;
	/// std::queue<uint16> qBrushfire;
	/// std::queue<uint16> qCull;
	/// for (auto i : oneExitRoomIndexes)
	/// {
	/// 	qBrushfire.push(i);
	/// 	visitedRoomIndexes.insert(i);
	/// }
	/// auto getNeighborIndexes = [&](uint16 i)->TArray<uint16>
	/// {
	/// 	TArray<uint16> retVal;
	/// 	const uint8 ix = i % ROOM_ARRAY_SIZE;
	/// 	const uint8 iy = i / ROOM_ARRAY_SIZE;
	/// 	auto& iNode = finalLevelLayout[i];
	/// 	if (iNode.hasNorth) retVal.Push(ix + (iy - 1) * ROOM_ARRAY_SIZE);
	/// 	if (iNode.hasSouth) retVal.Push(ix + (iy + 1) * ROOM_ARRAY_SIZE);
	/// 	if (iNode.hasEast)  retVal.Push(ix + 1 + iy * ROOM_ARRAY_SIZE);
	/// 	if (iNode.hasWest)  retVal.Push(ix - 1 + iy * ROOM_ARRAY_SIZE);
	/// 	return retVal;
	/// };
	/// while (!qBrushfire.empty())
	/// {
	/// 	const uint16 nextI = qBrushfire.front();
	/// 	qBrushfire.pop();
	/// 	qCull.push(nextI);
	/// 	// iterate over each neighbor index
	/// 	//	if the neighbor hasn't been visited yet
	/// 	//		visit the neighbor
	/// 	//		add neighbor to the brushfire Q
	/// 	const TArray<uint16> nextNeighbors = getNeighborIndexes(nextI);
	/// 	for (uint16 ni : nextNeighbors)
	/// 	{
	/// 		if (visitedRoomIndexes.find(ni) != visitedRoomIndexes.end())
	/// 		{
	/// 			continue;
	/// 		}
	/// 		visitedRoomIndexes.insert(ni);
	/// 		qBrushfire.push(ni);
	/// 	}
	/// }
	/// // at this point, qCull contains the queue of room indexes to cull
	//-Cull rooms based on floorNumber using the list
	static const int NUM_ROOMS_ON_FIRST_FLOOR = 5;
	const int numRoomsToCull = (ROOM_ARRAY_SIZE*ROOM_ARRAY_SIZE) - 
		NUM_ROOMS_ON_FIRST_FLOOR - floorNumber;
	normalRoomsTotal = finalLevelLayout.Num() - numRoomsToCull - 2;
	for (int r = 0; r < numRoomsToCull; r++)
	{
		TArray<uint16> oneExitRoomIndexes = locateOneExitRooms();
		if (!ensure(oneExitRoomIndexes.Num() >= 1))
		{
			// Should never happen, but JUST IN CASE
			break;
		}
		const uint16 randOneRoomIndexI =
			FMath::RandRange(0, oneExitRoomIndexes.Num() - 1);
		const uint16 nextCullI = oneExitRoomIndexes[randOneRoomIndexI];
		auto& cullNode = finalLevelLayout[nextCullI];
		// to cull a room, we remove all the exits for the room, and then
		//	iterate over all the adjacent rooms and remove their exits to this
		//	room
		cullNode.hasNorth = cullNode.hasEast = cullNode.hasSouth = 
			cullNode.hasWest = false;
		const uint8 cullX = nextCullI % ROOM_ARRAY_SIZE;
		const uint8 cullY = nextCullI / ROOM_ARRAY_SIZE;
		if (cullX > 0)
		{
			const uint16 neighborI = cullX - 1 + cullY * ROOM_ARRAY_SIZE;
			auto& neighborNode     = finalLevelLayout[neighborI];
			neighborNode.hasEast   = false;
		}
		if (cullX < ROOM_ARRAY_SIZE - 1)
		{
			const uint16 neighborI = cullX + 1 + cullY * ROOM_ARRAY_SIZE;
			auto& neighborNode     = finalLevelLayout[neighborI];
			neighborNode.hasWest   = false;
		}
		if (cullY > 0)
		{
			const uint16 neighborI = cullX + (cullY - 1) * ROOM_ARRAY_SIZE;
			auto& neighborNode     = finalLevelLayout[neighborI];
			neighborNode.hasSouth  = false;
		}
		if (cullY < ROOM_ARRAY_SIZE - 1)
		{
			const uint16 neighborI = cullX + (cullY + 1) * ROOM_ARRAY_SIZE;
			auto& neighborNode     = finalLevelLayout[neighborI];
			neighborNode.hasNorth  = false;
		}
	}
	//-Iterate over the finalLevelLayout to find the rooms with one exit again
	TArray<uint16> oneExitRoomIndexes = locateOneExitRooms();
	//-If there are no 1-exit rooms, choose a random room and remove all but 1 
	//	exit
	if (oneExitRoomIndexes.Num() == 0)
	{
		///TODO: is this even possible with a min-spanning tree that gets
		///	culled from the leaves?..
	}
	check(oneExitRoomIndexes.Num() > 0);
	//-Of the remaining rooms with one exit, choose one to be a boss room
	auto& bossNode = finalLevelLayout[oneExitRoomIndexes[0]];
	bossNode.bossRoom = true;
	//-Of the remaining rooms that aren't the boss room, choose one to be the
	//	starting room
	// iterate over finalLevelLayout and accumulate a list of rooms that have
	//	at least ONE exit, and are not a boss room
	// Then, choose a random room as the start
	TArray<int32> startCandidateIndexes;
	for (int32 i = 0; i < finalLevelLayout.Num(); i++)
	{
		const auto& node = finalLevelLayout[i];
		if (node.exitCount() < 1 ||
			node.bossRoom)
		{
			continue;
		}
		startCandidateIndexes.Push(i);
	}
	check(startCandidateIndexes.Num() > 0);
	const int32 startI = startCandidateIndexes[
		FMath::Rand() % startCandidateIndexes.Num()];
	startCoord = { 
		uint8(startI%ROOM_ARRAY_SIZE),
		uint8(startI/ROOM_ARRAY_SIZE) };
	currCoord = startCoord;
	// Before loading the actual level streams, we need to figure out how many
	//	of each type of room there are in the project //
	const TArray<TArray<FString>> uniqueRoomNamesPerType = 
		findUniqueRoomNamesPerType();
	// Finally, actually load the Unreal Engine Levels ////////////////////////
	for (uint8 y = 0; y < ROOM_ARRAY_SIZE; y++)
	{
		for (uint8 x = 0; x < ROOM_ARRAY_SIZE; x++)
		{
			const int32 i = x + y*ROOM_ARRAY_SIZE;
			auto& node = finalLevelLayout[i];
			/// DEBUG: reveal entire level with this line /////////////////////
			///if(node.exitCount() > 0) node.hasBeenVisited = true;
			/// ///////////////////////////////////////////////////////////////
			const RoomCoord rc{ x,y };
			FString levelDir = findLevelDir(node);
			const FQuat levelQuat = findLevelRotation(node);
			const FString roomNamePostfix = rc == startCoord ?
				FString("S") : node.bossRoom ?
				FString("B") : 
				findUniqueRoomName(node, uniqueRoomNamesPerType);
			// we now should know what type of room to load (levelDir)
			//	as well as where we need to put it (r & c)
			const FString roomName = levelDir + roomNamePostfix;
			///if (rc == startCoord)
			///{
			///	roomName = levelDir + FString("S");
			///}
			///else if (node.bossRoom)
			///{
			///	roomName = levelDir + FString("B");
			///}
			const FString uniqueRoomInstanceString =
				"room" + FString::FromInt(y*ROOM_ARRAY_SIZE + x);
			UE_LOG(LogTemp, Warning, 
				TEXT("x=%d y=%d roomName='%s' uniqueRoomInstanceString='%s'"),
				x,y, *roomName, *uniqueRoomInstanceString);
			node.uniqueLevelName = uniqueRoomInstanceString;
			//UE_LOG(LogTemp, Warning, 
			//	TEXT("uniqueRoomInstanceString='%s'"), *uniqueRoomInstanceString);
			gm->createLevelInstance(FName(*roomName), uniqueRoomInstanceString);
			ULevelStreaming* levelStreaming = UGameplayStatics::GetStreamingLevel(
				world, FName(*uniqueRoomInstanceString));
			if (levelStreaming)
			{
				if (rc == startCoord)
				{
					FLatentActionInfo latentInfo;
					latentInfo.CallbackTarget = this;
					latentInfo.ExecutionFunction = FName("onStartLevelStreamLoaded");
					latentInfo.Linkage = 1;
					UGameplayStatics::LoadStreamLevel(world,
						FName(*uniqueRoomInstanceString), true, true, latentInfo);
					node.hasBeenVisited = true;
				}
				const FVector roomLocation(x*ROOM_SIZE, y*ROOM_SIZE, 0);
				levelStreaming->LevelTransform.SetLocation(roomLocation);
				levelStreaming->LevelTransform.SetRotation(levelQuat);
			}
		}
	}
	normalRoomsUnexplored = normalRoomsTotal;
}
ULevelMap::RoomCoord ULevelMap::getStartCoord() const
{
	return startCoord;
}
ULevelMap::RoomCoord ULevelMap::getCurrCoord() const
{
	return currCoord;
}
bool ULevelMap::advanceCurrCoord(const FVector & exitVec)
{
	int8 offsetX = 0, offsetY = 0;
	exitVecToOffsets(exitVec, offsetX, offsetY);
	currCoord = RoomCoord{ uint8(currCoord.x() + offsetX), uint8(currCoord.y() + offsetY) };
	check(currCoord.x() < ROOM_ARRAY_SIZE && currCoord.y() < ROOM_ARRAY_SIZE);
	const int32 currI = currCoord.x() + currCoord.y()*ROOM_ARRAY_SIZE;
	FLevelGenNode& currNode = finalLevelLayout[currI];
	bool wasPreviouslyVisited = currNode.hasBeenVisited;
	currNode.hasBeenVisited = true;
	if (!wasPreviouslyVisited)
	{
		normalRoomsUnexplored--;
	}
	return !wasPreviouslyVisited;
}
FString ULevelMap::currentRoomLevelName()
{
	const int32 currI = currCoord.x() + currCoord.y()*ROOM_ARRAY_SIZE;
	FLevelGenNode& currNode = finalLevelLayout[currI];
	return currNode.uniqueLevelName;
}
FString ULevelMap::adjacentRoomLevelName(const FVector & exitVec) const
{
	int8 offsetX = 0, offsetY = 0;
	exitVecToOffsets(exitVec, offsetX, offsetY);
	int8 adjacentX = currCoord.x() + offsetX;
	int8 adjacentY = currCoord.y() + offsetY;
	if (adjacentX < 0 || adjacentX >= ROOM_ARRAY_SIZE ||
		adjacentY < 0 || adjacentY >= ROOM_ARRAY_SIZE)
	{
		return "INVALID-NAME";
	}
	const int32 adjacentI = adjacentX + adjacentY*ROOM_ARRAY_SIZE;
	return finalLevelLayout[adjacentI].uniqueLevelName;
}
bool ULevelMap::adjacentRoomIsBossRoom(const FVector& exitVec) const
{
	int8 offsetX = 0, offsetY = 0;
	exitVecToOffsets(exitVec, offsetX, offsetY);
	int8 adjacentX = currCoord.x() + offsetX;
	int8 adjacentY = currCoord.y() + offsetY;
	if (adjacentX < 0 || adjacentX >= ROOM_ARRAY_SIZE ||
		adjacentY < 0 || adjacentY >= ROOM_ARRAY_SIZE)
	{
		return false;
	}
	const int32 adjacentI = adjacentX + adjacentY * ROOM_ARRAY_SIZE;
	return finalLevelLayout[adjacentI].bossRoom;
}
void ULevelMap::addActorToCurrentRoom(AActor * actor)
{
	const int32 currI = currCoord.x() + currCoord.y()*ROOM_ARRAY_SIZE;
	if (currI >= finalLevelLayout.Num())
	{
		return;
	}
	FLevelGenNode& currNode = finalLevelLayout[currI];
	currNode.actorsOwnedByThisRoom.AddUnique(actor);
}
TArray<TWeakObjectPtr<AActor>> ULevelMap::getCurrentRoomActorSet() const
{
	const int32 currI = currCoord.x() + currCoord.y()*ROOM_ARRAY_SIZE;
	if (currI >= finalLevelLayout.Num())
	{
		return TArray<TWeakObjectPtr<AActor>>{};
	}
	const FLevelGenNode& currNode = finalLevelLayout[currI];
	return TArray<TWeakObjectPtr<AActor>>{currNode.actorsOwnedByThisRoom};
}
FVector ULevelMap::currentRoomWorldOffset() const
{
	return FVector{ currCoord.x()*ROOM_SIZE, currCoord.y()*ROOM_SIZE, 0 };
}
bool ULevelMap::isVisited(const RoomCoord& coord) const
{
	const int32 roomI = coord.x() + coord.y() * ROOM_ARRAY_SIZE;
	return finalLevelLayout[roomI].hasBeenVisited;
}
bool ULevelMap::hasExitNorth(const RoomCoord& coord) const
{
	const int32 roomI = coord.x() + coord.y() * ROOM_ARRAY_SIZE;
	return finalLevelLayout[roomI].hasNorth;
}
bool ULevelMap::hasExitEast(const RoomCoord& coord) const
{
	const int32 roomI = coord.x() + coord.y() * ROOM_ARRAY_SIZE;
	return finalLevelLayout[roomI].hasEast;
}
bool ULevelMap::hasExitSouth(const RoomCoord& coord) const
{
	const int32 roomI = coord.x() + coord.y() * ROOM_ARRAY_SIZE;
	return finalLevelLayout[roomI].hasSouth;
}
bool ULevelMap::hasExitWest(const RoomCoord& coord) const
{
	const int32 roomI = coord.x() + coord.y() * ROOM_ARRAY_SIZE;
	return finalLevelLayout[roomI].hasWest;
}
int ULevelMap::getFloorNumber() const
{
	return floorNumber;
}
uint16 ULevelMap::getTotalNormalRooms() const
{
	return normalRoomsTotal;
}
uint16 ULevelMap::getTotalNormalUnexploredRooms() const
{
	return normalRoomsUnexplored;
}
bool ULevelMap::getHasSpawnedKeyYet() const
{
	return hasSpawnedKeyYet;
}
void ULevelMap::setHasSpawnedKeyYet(bool value)
{
	hasSpawnedKeyYet = value;
}
bool ULevelMap::getHasSpawnedBossDoorYet() const
{
	return hasSpawnedBossDoorYet;
}
void ULevelMap::setHasSpawnedBossDoorYet(bool value)
{
	hasSpawnedBossDoorYet = value;
}
TArray<TArray<FString>> ULevelMap::findUniqueRoomNamesPerType() const
{
	static const TArray<FString> ROOM_DIRECTORIES = {
		TEXT("/Game/levels/proceduralRooms/N"   ),
		TEXT("/Game/levels/proceduralRooms/NS"  ),
		TEXT("/Game/levels/proceduralRooms/NE"  ),
		TEXT("/Game/levels/proceduralRooms/NES" ),
		TEXT("/Game/levels/proceduralRooms/NESW")
	};
	TArray<TArray<FString>> retVal;
	for (auto& dir : ROOM_DIRECTORIES)
	{
		retVal.Add({});
		UObjectLibrary* objLib = UObjectLibrary::CreateLibrary(
			UWorld::StaticClass(), false, true);
		objLib->LoadAssetDataFromPath(dir);
		TArray<FAssetData> assetData;
		objLib->GetAssetDataList(assetData);
		for (auto& assetDat : assetData)
		{
			///UE_LOG(LogTemp, Warning, TEXT("found map \"%s\""),
			///	*assetDat.AssetName.ToString());
			FString trimmedName;
			assetDat.AssetName.ToString().Split(
				TEXT("-"), nullptr, &trimmedName);
			if (trimmedName == TEXT("B") ||
				trimmedName == TEXT("S") ||
				trimmedName == TEXT("TEMPLATE"))
			{
				continue;
			}
			retVal.Last().Add(trimmedName);
		}
	}
	return retVal;
}
/// TArray<uint32> ULevelMap::findNumberOfUniqueRoomsPerType()
/// {
/// 	TArray<uint32> retVal;
/// 	UObjectLibrary* objLib = UObjectLibrary::CreateLibrary(
/// 		UWorld::StaticClass(), false, true);
/// 	objLib->LoadAssetDataFromPath(TEXT("/Game/levels/proceduralRooms/N"));
/// 	TArray<FAssetData> assetData;
/// 	objLib->GetAssetDataList(assetData);
/// 	///for (auto& assetDat : assetData)
/// 	///{
/// 	///	UE_LOG(LogTemp, Warning, TEXT("found map \"%s\""),
/// 	///		*assetDat.AssetName.ToString());
/// 	///	///assetDat.AssetName.ToString();
/// 	///}
/// 	return retVal;
/// }
FString ULevelMap::findUniqueRoomName(const FLevelGenNode& node,
	TArray<TArray<FString>>const& numUniqueRoomNamesPerType) const
{
	int8 numEdges = 0;
	if (node.hasNorth) numEdges++;
	if (node.hasSouth) numEdges++;
	if (node.hasEast ) numEdges++;
	if (node.hasWest ) numEdges++;
	int32 roomNameArrayIndex;
	switch (numEdges)
	{
	case 1:
		roomNameArrayIndex = 0;
		break;
	case 2:
		if ((node.hasNorth && node.hasSouth) ||
			(node.hasEast && node.hasWest))
		{
			roomNameArrayIndex = 1;
			break;
		}
		roomNameArrayIndex = 2;
		break;
	case 3:
		roomNameArrayIndex = 3;
		break;
	case 4:
	default:
		roomNameArrayIndex = 4;
		break;
	}
	const int32 randIndex = FMath::RandRange(0, 
		numUniqueRoomNamesPerType[roomNameArrayIndex].Num() - 1);
	return numUniqueRoomNamesPerType[roomNameArrayIndex][randIndex];
}
FString ULevelMap::findLevelDir(const FLevelGenNode & node)
{
	int8 numEdges = 0;
	if (node.hasNorth) numEdges++;
	if (node.hasSouth) numEdges++;
	if (node.hasEast ) numEdges++;
	if (node.hasWest ) numEdges++;
	switch (numEdges)
	{
	case 1:
		return "/Game/levels/proceduralRooms/N/N-";
	case 2:
		if ((node.hasNorth && node.hasSouth) ||
			(node.hasEast && node.hasWest))
		{
			return "/Game/levels/proceduralRooms/NS/NS-";
		}
		return "/Game/levels/proceduralRooms/NE/NE-";
	case 3:
		return "/Game/levels/proceduralRooms/NES/NES-";
	case 4:
	default:
		// By default, just select a room with all 4 exits, YOLO
		return "/Game/levels/proceduralRooms/NESW/NESW-";
	}
}
FQuat ULevelMap::findLevelRotation(const FLevelGenNode& node)
{
	int8 numEdges = 0;
	if (node.hasNorth) numEdges++;
	if (node.hasSouth) numEdges++;
	if (node.hasEast ) numEdges++;
	if (node.hasWest ) numEdges++;
	switch (numEdges)
	{
	case 1:
		if (node.hasNorth)
		{
			return FQuat::Identity;
		}
		else if (node.hasEast)
		{
			return FQuat{ FRotator{0, 90, 0} };
		}
		else if (node.hasSouth)
		{
			return FQuat{ FRotator{ 0, 180, 0 } };
		}
		//else node.hasWest
		return FQuat{ FRotator{ 0, -90, 0 } };
	case 2:
		if (node.hasNorth && node.hasSouth)
		{
			return FQuat::Identity;
		}
		else if (node.hasEast && node.hasWest)
		{
			return FQuat{ FRotator{ 0, 90, 0 } };
		}
		else
		{
			if (node.hasNorth && node.hasEast)
			{
				return FQuat::Identity;
			}
			else if (node.hasEast && node.hasSouth)
			{
				return FQuat{ FRotator{ 0, 90, 0 } };
			}
			else if (node.hasSouth && node.hasWest)
			{
				return FQuat{ FRotator{ 0, 180, 0 } };
			}
			//else if (node.hasWest && node.hasNorth)
			return FQuat{ FRotator{ 0, -90, 0 } };
		}
		break;
	case 3:
		if (node.hasNorth && node.hasEast && node.hasSouth)
		{
			return FQuat::Identity;
		}
		else if (node.hasEast && node.hasSouth && node.hasWest)
		{
			return FQuat{ FRotator{ 0, 90, 0 } };
		}
		else if (node.hasSouth && node.hasWest && node.hasNorth)
		{
			return FQuat{ FRotator{ 0, 180, 0 } };
		}
		//else if (node.hasWest && node.hasNorth && node.hasNorth)
		return FQuat{ FRotator{ 0, -90, 0 } };
	case 4:
	default:
		return FQuat{ FRotator{ 0, -90.f + 90*FMath::RandRange(0,3), 0 } };
	}
}
void ULevelMap::exitVecToOffsets(const FVector & exitVec,
	int8 & outOffsetX, int8 & outOffsetY) const
{
	outOffsetX = outOffsetY = 0;
	if (FMath::Abs(exitVec.X) > FMath::Abs(exitVec.Y))
	{
		if (exitVec.X > 0)
		{
			// EAST //
			outOffsetX = 1;
		}
		else
		{
			// WEST //
			outOffsetX = -1;
		}
	}
	else
	{
		if (exitVec.Y > 0)
		{
			// SOUTH //
			outOffsetY = 1;
		}
		else
		{
			// NORTH //
			outOffsetY = -1;
		}
	}
}
void ULevelMap::onStartLevelStreamLoaded()
{
	/// Prevent player from spawning at the level's APlayerSpawn actor ////////
	///if (floorNumber == 0)
	///{
	///	return;///DEBUG - delete me when done testing
	///}
	/// ///////////////////////////////////////////////////////////////////////
	// Need to set the Player's starting position manually 
	//	when the first Level is loaded, because the APlayerStart located
	//	in the starting room & isn't spawned when the player's Pawn is spawned! //
	auto world = GetWorld();
	check(world);
	AGameModeBase* gmb = world->GetAuthGameMode();
	check(gmb);
	Airone3dGameMode* gm = Cast<Airone3dGameMode>(gmb);
	check(gm);
	gm->fadeIn(FLinearColor{ 0,0,0 }, 1.f);
	/// ASSUMPTION: only one player per game:
	APlayerController* pc = world->GetFirstPlayerController();
	check(pc);
	APawn* pPawn = pc->GetPawn();
	check(pPawn);
	//TSet<AActor*> currRoomActorSet = getCurrentRoomActorSet();
	//for (auto& actor : currRoomActorSet)
	//{
	//	UE_LOG(LogTemp, Warning,
	//		TEXT("initial room actors - actIt->GetName()=%s"),
	//		*actor->GetName())
	//}
	for (TObjectIterator<AActor> actIt; actIt; ++actIt)
	{
		if (actIt->GetWorld() != world)
		{
			continue;
		}
		//UE_LOG(LogTemp, Warning,
		//	TEXT("initial room actors - actIt->GetName()=%s"),
		//	*actIt->GetName())
		APlayerStart* pStart = Cast<APlayerStart>(*actIt);
		if (pStart)
		{
			pPawn->SetActorLocation(pStart->GetActorLocation());
			break;
		}
	}
}
