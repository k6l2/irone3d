// YOLO SWAG 420
#include "LevelMap.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/Engine/LevelStreaming.h>
#include <Runtime/CoreUObject/Public/UObject/UObjectIterator.h>
#include <Runtime/Engine/Classes/GameFramework/PlayerStart.h>
#include <set>
#include <tuple>
#include "irone3dGameMode.h"
const uint8 ULevelMap::ROOM_ARRAY_SIZE = 5;
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
ULevelMap::LevelGenNode::LevelGenNode()
	:hasNorth(false)
	,hasSouth(false)
	,hasEast(false)
	,hasWest(false)
	,hasBeenVisited(false)
	,uniqueLevelName("UNINITIALIZED-NAME")
{
}
void ULevelMap::generateNewLevel(UWorld* world, int8 floorNumber)
{
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
	finalLevelLayout.SetNum(ROOM_ARRAY_SIZE);
	for (int32 y = 0; y < ROOM_ARRAY_SIZE; y++)
	{
		finalLevelLayout[y].SetNum(ROOM_ARRAY_SIZE);
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
		int32 fromRow = edge.indexFrom / ROOM_ARRAY_SIZE;
		int32 fromCol = edge.indexFrom % ROOM_ARRAY_SIZE;
		int32 toRow = edge.indexTo / ROOM_ARRAY_SIZE;
		int32 toCol = edge.indexTo   % ROOM_ARRAY_SIZE;
		if (fromRow == toRow)
		{
			if (fromCol < toCol)
			{
				finalLevelLayout[fromRow][fromCol].hasEast = true;
				finalLevelLayout[toRow][toCol].hasWest = true;
			}
			else
			{
				check(fromCol > toCol);
				finalLevelLayout[fromRow][fromCol].hasWest = true;
				finalLevelLayout[toRow][toCol].hasEast = true;
			}
		}
		else
		{
			check(fromCol == toCol);
			if (fromRow < toRow)
			{
				finalLevelLayout[fromRow][fromCol].hasSouth = true;
				finalLevelLayout[toRow][toCol].hasNorth = true;
			}
			else
			{
				check(fromRow > toRow);
				finalLevelLayout[fromRow][fromCol].hasNorth = true;
				finalLevelLayout[toRow][toCol].hasSouth = true;
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
	// at this point, finalLevelLayout contains our minimum spanning tree
	///TODO: procedurally randomize room types & place a boss room
	///TODO: procedurally find the starting room
	startCoord = { 0,0 };
	currCoord = startCoord;
	for (uint8 y = 0; y < ROOM_ARRAY_SIZE; y++)
	{
		for (uint8 x = 0; x < ROOM_ARRAY_SIZE; x++)
		{
			auto& node = finalLevelLayout[y][x];
			const RoomCoord rc{ x,y };
			FString levelDir = findLevelDir(node);
			// we now should know what type of room to load (levelDir)
			//	as well as where we need to put it (r & c)
			FString roomName = levelDir + FString::FromInt(0);
			if (rc == startCoord)
			{
				roomName = levelDir + FString("S");
			}
			//UE_LOG(LogTemp, Warning, 
			//	TEXT("x=%d y=%d roomName='%s'"), x,y, *roomName);
			const FString uniqueRoomInstanceString =
				"room" + FString::FromInt(y*ROOM_ARRAY_SIZE + x);
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
				static const float ROOM_SIZE = 2500;
				const FVector roomLocation(x*ROOM_SIZE, y*ROOM_SIZE, 0);
				levelStreaming->LevelTransform.SetLocation(roomLocation);
			}
		}
	}
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
	LevelGenNode& currNode = finalLevelLayout[currCoord.y()][currCoord.x()];
	bool wasPreviouslyVisited = currNode.hasBeenVisited;
	currNode.hasBeenVisited = true;
	return !wasPreviouslyVisited;
}
FString ULevelMap::currentRoomLevelName()
{
	LevelGenNode& currNode = finalLevelLayout[currCoord.y()][currCoord.x()];
	return currNode.uniqueLevelName;
}
FString ULevelMap::adjacentRoomLevelName(const FVector & exitVec)
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
	return finalLevelLayout[adjacentY][adjacentX].uniqueLevelName;
}
void ULevelMap::addActorToCurrentRoom(AActor * actor)
{
	LevelGenNode& currNode = finalLevelLayout[currCoord.y()][currCoord.x()];
	currNode.actorsOwnedByThisRoom.Add(actor);
}
TSet<AActor*> ULevelMap::getCurrentRoomActorSet() const
{
	const LevelGenNode& currNode = finalLevelLayout[currCoord.y()][currCoord.x()];
	return TSet<AActor*>{currNode.actorsOwnedByThisRoom};
}
FString ULevelMap::findLevelDir(const LevelGenNode & node)
{
	int8 numEdges = 0;
	if (node.hasNorth) numEdges++;
	if (node.hasSouth) numEdges++;
	if (node.hasEast) numEdges++;
	if (node.hasWest) numEdges++;
	FString levelDir = "/Game/levels/proceduralRooms/NESW/NESW-";
	switch (numEdges)
	{
	case 1:
		if (node.hasNorth)
		{
			levelDir = "/Game/levels/proceduralRooms/N/N-";
		}
		else if (node.hasSouth)
		{
			levelDir = "/Game/levels/proceduralRooms/S/S-";
		}
		else if (node.hasEast)
		{
			levelDir = "/Game/levels/proceduralRooms/E/E-";
		}
		else
		{
			check(node.hasWest);
			levelDir = "/Game/levels/proceduralRooms/W/W-";
		}
		break;
	case 2:
		if (node.hasNorth && node.hasSouth)
		{
			levelDir = "/Game/levels/proceduralRooms/NS/NS-";
		}
		else if (node.hasEast && node.hasWest)
		{
			levelDir = "/Game/levels/proceduralRooms/WE/WE-";
		}
		else if (node.hasEast && node.hasSouth)
		{
			levelDir = "/Game/levels/proceduralRooms/ES/ES-";
		}
		else if (node.hasNorth && node.hasEast)
		{
			levelDir = "/Game/levels/proceduralRooms/NE/NE-";
		}
		else if (node.hasNorth && node.hasWest)
		{
			levelDir = "/Game/levels/proceduralRooms/NW/NW-";
		}
		else
		{
			check(node.hasSouth && node.hasWest);
			levelDir = "/Game/levels/proceduralRooms/SW/SW-";
		}
		break;
	case 3:
		if (node.hasEast && node.hasSouth && node.hasWest)
		{
			levelDir = "/Game/levels/proceduralRooms/ESW/ESW-";
		}
		else if (node.hasNorth && node.hasEast && node.hasSouth)
		{
			levelDir = "/Game/levels/proceduralRooms/NES/NES-";
		}
		else if (node.hasNorth && node.hasEast && node.hasWest)
		{
			levelDir = "/Game/levels/proceduralRooms/NEW/NEW-";
		}
		else
		{
			check(node.hasSouth && node.hasWest && node.hasNorth);
			levelDir = "/Game/levels/proceduralRooms/SWN/SWN-";
		}
		break;
	case 4:
		break;
	default:
		check(false);
		break;
	}
	return levelDir;
}
void ULevelMap::exitVecToOffsets(const FVector & exitVec, int8 & outOffsetX, int8 & outOffsetY)
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
