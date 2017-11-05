#include "irone3dGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "Runtime/Engine/Classes/Engine/LevelStreaming.h"
#include <set>
Airone3dGameMode::Airone3dGameMode()
{
	// set default pawn class to our Blueprinted character //
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
void Airone3dGameMode::InitGame(const FString & MapName, 
	const FString & Options, FString & ErrorMessage)
{
	static const unsigned ROOM_ARRAY_SIZE = 5;
	Super::InitGame(MapName, Options, ErrorMessage);
	auto world = GetWorld();
	if (!world)
	{
		return;
	}
	struct LevelGenEdge
	{
		// Even though these names suggest edge directionality,
		//	this algorithm treats these edges as bi-directional.
		int32 indexFrom;
		int32 indexTo;
		float weight;
	};
	struct LevelGenNode
	{
		bool hasNorth;
		bool hasSouth;
		bool hasEast;
		bool hasWest;
	};
	// Build a list of all edges with randomized weights
	TArray<LevelGenEdge> edgeList;
	for (int32 r = 0; r < ROOM_ARRAY_SIZE; r++)
	{
		for (int32 c = 0; c < ROOM_ARRAY_SIZE; c++)
		{
			LevelGenEdge newEdge;
			newEdge.indexFrom = r*ROOM_ARRAY_SIZE + c;
			newEdge.indexTo = r*ROOM_ARRAY_SIZE + (c + 1);
			newEdge.weight = FMath::RandRange(0.f, 1.f);
			if (c + 1 < ROOM_ARRAY_SIZE)
			{
				edgeList.Add(newEdge);
			}
			newEdge.indexTo = (r+1)*ROOM_ARRAY_SIZE + c;
			newEdge.weight = FMath::RandRange(0.f, 1.f);
			if (r + 1 < ROOM_ARRAY_SIZE)
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
	TArray<TArray<LevelGenNode>> finalLevelLayout;
	finalLevelLayout.SetNum(ROOM_ARRAY_SIZE);
	for (int32 r = 0; r < ROOM_ARRAY_SIZE; r++)
	{
		finalLevelLayout[r].SetNum(ROOM_ARRAY_SIZE);
		for (int32 c = 0; c < ROOM_ARRAY_SIZE; c++)
		{
			finalLevelLayout[r][c].hasNorth = false;
			finalLevelLayout[r][c].hasSouth = false;
			finalLevelLayout[r][c].hasEast  = false;
			finalLevelLayout[r][c].hasWest  = false;
		}
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
		int32 toRow   = edge.indexTo   / ROOM_ARRAY_SIZE;
		int32 toCol   = edge.indexTo   % ROOM_ARRAY_SIZE;
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
				 indexToIt   != pickedNodeIndexes.end()) ||
				(indexFromIt != pickedNodeIndexes.end() &&
				 indexToIt   == pickedNodeIndexes.end()))
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
	for (int32 r = 0; r < ROOM_ARRAY_SIZE; r++)
	{
		for (int32 c = 0; c < ROOM_ARRAY_SIZE; c++)
		{
			auto& node = finalLevelLayout[r][c];
			int8 numEdges = 0;
			if (node.hasNorth) numEdges++;
			if (node.hasSouth) numEdges++;
			if (node.hasEast) numEdges++;
			if (node.hasWest) numEdges++;
			FString levelDir = "/Game/levels/proceduralRooms/NESW/";
			switch (numEdges)
			{
			case 1:
				if (node.hasNorth)
				{
					levelDir = "/Game/levels/proceduralRooms/N/";
				}
				else if (node.hasSouth)
				{
					levelDir = "/Game/levels/proceduralRooms/S/";
				}
				else if (node.hasEast)
				{
					levelDir = "/Game/levels/proceduralRooms/E/";
				}
				else
				{
					check(node.hasWest);
					levelDir = "/Game/levels/proceduralRooms/W/";
				}
				break;
			case 2:
				if (node.hasNorth && node.hasSouth)
				{
					levelDir = "/Game/levels/proceduralRooms/NS/";
				}
				else if (node.hasEast && node.hasWest)
				{
					levelDir = "/Game/levels/proceduralRooms/WE/";
				}
				else if (node.hasEast && node.hasSouth)
				{
					levelDir = "/Game/levels/proceduralRooms/ES/";
				}
				else if (node.hasNorth && node.hasEast)
				{
					levelDir = "/Game/levels/proceduralRooms/NE/";
				}
				else if (node.hasNorth && node.hasWest)
				{
					levelDir = "/Game/levels/proceduralRooms/NW/";
				}
				else
				{
					check(node.hasSouth && node.hasWest);
					levelDir = "/Game/levels/proceduralRooms/SW/";
				}
				break;
			case 3:
				if (node.hasEast && node.hasSouth && node.hasWest)
				{
					levelDir = "/Game/levels/proceduralRooms/ESW/";
				}
				else if (node.hasNorth && node.hasEast && node.hasSouth)
				{
					levelDir = "/Game/levels/proceduralRooms/NES/";
				}
				else if (node.hasNorth && node.hasEast && node.hasWest)
				{
					levelDir = "/Game/levels/proceduralRooms/NEW/";
				}
				else
				{
					check(node.hasSouth && node.hasWest && node.hasNorth);
					levelDir = "/Game/levels/proceduralRooms/SWN/";
				}
				break;
			case 4:
				break;
			default:
				check(false);
				break;
			}
			// we now should know what type of room to load (levelDir)
			//	as well as where we need to put it (r & c)
			const FString roomName = levelDir + FString::FromInt(0);
			const FString uniqueRoomInstanceString = 
				"room" + FString::FromInt(r*ROOM_ARRAY_SIZE+c);
			//UE_LOG(LogTemp, Warning, 
			//	TEXT("uniqueRoomInstanceString='%s'"), *uniqueRoomInstanceString);
			createLevelInstance(FName(*roomName), uniqueRoomInstanceString);
			ULevelStreaming* levelStreaming = UGameplayStatics::GetStreamingLevel(
				world, FName(*uniqueRoomInstanceString));
			if (levelStreaming)
			{
				levelStreaming->bShouldBeLoaded = true;
				levelStreaming->bShouldBeVisible = true;
				static const float ROOM_SIZE = 2500;
				const FVector roomLocation(c*ROOM_SIZE, r*ROOM_SIZE, 0);
				levelStreaming->LevelTransform.SetLocation(roomLocation);
			}
		}
	}
}
