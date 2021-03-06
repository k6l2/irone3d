#include "irone3dGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/Camera/CameraActor.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Runtime/CoreUObject/Public/UObject/UObjectIterator.h>
//#include <Runtime/Engine/Classes/GameFramework/Pawn.h>
//#include <Runtime/Engine/Classes/GameFramework/PawnMovementComponent.h>
//#include <Runtime/Engine/Classes/GameFramework/Character.h>
//#include <Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h>
#include <Blueprint/UserWidget.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Engine/LevelStreaming.h>
//#include <Runtime/Engine/Classes/AI/Navigation/NavMeshBoundsVolume.h>
#include "Irone3dGameState.h"
#include "RoomTransitionTrigger.h"
#include "Irone3DPlayer.h"
#include "Irone3DPlayerController.h"
//#include <Runtime/Engine/Classes/AI/Navigation/NavigationSystem.h>
#include <Runtime/NavigationSystem/Public/NavMesh/NavMeshBoundsVolume.h>
#include <Runtime/NavigationSystem/Public/NavigationSystem.h>
#include "LevelTransitionTrigger.h"
#include "Irone3dGameInstance.h"
#include "Inventory.h"
#include "TurretLaser.h"
const FVector Airone3dGameMode::TRANSITION_CAM_OFFSET = { -110,160,200 };
const float Airone3dGameMode::LEVEL_TRANSITION_FADE_TIME = 0.75f;
Airone3dGameMode::Airone3dGameMode()
	:transitioning(false)
	,levelComplete(false)
	,fadeTime(-1.f)
{
	PrimaryActorTick.bCanEverTick = true;
	// set default pawn class to our Blueprinted character //
	//static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
	//	TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	//if (PlayerPawnBPClass.Class != NULL)
	//{
	//	DefaultPawnClass = PlayerPawnBPClass.Class;
	//}
}
void Airone3dGameMode::InitGame(const FString & MapName, 
	const FString & Options, FString & ErrorMessage)
{
	UE_LOG(LogTemp, Warning,
		TEXT("MapName=%s"), *MapName);
	Super::InitGame(MapName, Options, ErrorMessage);
	auto world = GetWorld();
	if (!world)
	{
		return;
	}
	// Need to spawn a persistent camera actor for room transitions //
	FActorSpawnParameters cameraSpawnParams;
	cameraSpawnParams.bNoFail = true;
	cameraSpawnParams.Name = TEXT("cameraRoomTransitions");
	transitionCamera = world->SpawnActor<ACameraActor>(cameraSpawnParams);
	// create HUD widget //
	if (ensure(classHUD))
	{
		widgetHUD = CreateWidget<UUserWidget>(world, classHUD);
		if (widgetHUD)
		{
			widgetHUD->AddToViewport();
		}
	}
}
void Airone3dGameMode::InitGameState()
{
	Super::InitGameState();
	UWorld*const world = GetWorld();
	if (!ensure(world)) return;
	// Generate the level map //
	// This is a hack for finding out if we are debugging a pre-loaded room:
	//	just find any loaded instances of room transition triggers //
	bool testRoomLoaded = false;
	for (TObjectIterator<AActor> actIt; actIt; ++actIt)
	{
		if (actIt->GetWorld() != world)
		{
			continue;
		}
		//UE_LOG(LogTemp, Warning, TEXT("InitGameState: actor name=%s"), *actIt->GetName());
		if (actIt->IsA(ARoomTransitionTrigger::StaticClass()))
		{
			testRoomLoaded = true;
			break;
		}
	}
///	UE_LOG(LogTemp, Warning, TEXT("A pre-loaded room's transition trigger has \
///been detected! Assuming we are doing debug testing!"));
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	if (gs)
	{
		gs->generateLevelMap(world, testRoomLoaded);
		UIrone3dGameInstance*const gi = 
			Cast<UIrone3dGameInstance>(GetGameInstance());
		if (gi)
		{
			gs->getInventory()->setItemCounts(gi->getItemCounts());
		}
		updateMinimapWidget(gs->getLevelMap());
	}
}
void Airone3dGameMode::StartPlay()
{
	Super::StartPlay();
	// Unreal engine's navmeshes suck and need to be forced to rebuild manually
	//	all the time, apparently //
	UWorld *const world = GetWorld();
	ensure(world);
	///if (testRoomLoaded)
	{
		UNavigationSystemV1* const navSys =
			FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
		if (navSys)
		{
			navSys->Build();
		}
	}
}
void Airone3dGameMode::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	UWorld*const world = GetWorld();
	ensure(world);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
	//	FString::Printf(TEXT("Airone3dGameMode::Tick????")));
	if (fadeTime > 0 && fadeTimeTotal > 0)
	{
		fadeTime = FMath::Max(fadeTime - deltaSeconds, 0.f);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
		//	FString::Printf(TEXT("fadeTime=%f/%f   deltaSeconds=%f"), fadeTime, fadeTimeTotal,deltaSeconds));
		float ratio = fadeTime / fadeTimeTotal;
		if (!fadingIn)
		{
			ratio = 1.f - ratio;
		}
		colorFullScreenTexture.A = ratio;
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
		//	FString::Printf(TEXT("colorFullScreenTexture.A=%f"), colorFullScreenTexture.A));
		setFullscreenTextureColor(colorFullScreenTexture);
		if (fadeTime <= 0)
		{
			// the fade effect has just ended //
			if (fadingIn)
			{
				// the final steps of the room transition are now moved to the
				//	onEndTransition function because the player has not 
				//	actually regained control of their character at this point
			}
			else
			{
				if (levelComplete)
				{
					UGameplayStatics::OpenLevel(world, "procGenPersist");
				}
				else
				{
					// set all the previous room's actors to be hidden! //
					TArray<TWeakObjectPtr<AActor>> currRoomActorSet = 
						gs->getCurrentRoomActorSet();
					for (auto& actor : currRoomActorSet)
					{
						if (!actor.IsValid())
						{
							UE_LOG(LogTemp, Warning,
								TEXT("actor is invalid, skipping..."))
								continue;
						}
						UE_LOG(LogTemp, Warning,
							TEXT("setting actIt->GetName()=%s to be hidden!"),
							*actor->GetName());
						actor->SetActorHiddenInGame(true);
					}
					//9 ) set the new room's "hasBeenVisited" flag
					justLoadedRoom = gs->advanceCurrCoord(exitVec);
					updateMinimapWidget(gs->getLevelMap());
					//10) Load the destination level
					FLatentActionInfo latentInfo;
					latentInfo.CallbackTarget = this;
					latentInfo.ExecutionFunction = 
						FName("onLoadStreamLevelFinished");
					latentInfo.Linkage = 1;
					UGameplayStatics::LoadStreamLevel(world, 
						FName(*strTransitionLevelGoingTo), true, true, 
						latentInfo);
				}
			}
		}
		else // we're still in the process of fading out
		{
			if (levelComplete)
			{
				APlayerController*const pc = world->GetFirstPlayerController();
				ensure(pc);
				AIrone3DPlayer*const player = 
					Cast<AIrone3DPlayer>(pc->GetPawn());
				ensure(player);
				FVector toPlayerVec = player->GetActorLocation() - 
					transitionCamera->GetActorLocation();
				transitionCamera->SetActorRotation(toPlayerVec.Rotation());
			}
		}
	}
}
void Airone3dGameMode::startRoomTransition(ARoomTransitionTrigger* trigger)
{
	if (transitioning)
	{
		return;
	}
	UWorld*const world = GetWorld();
	ensure(world);
	/// ASSUMPTION: only one player per game:
	APlayerController*const pc = world->GetFirstPlayerController();
	ensure(pc);
	APawn*const pPawn = pc->GetPawn();
	ensure(pPawn);
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(pPawn);
	ensure(player);
	AIrone3DPlayerController*const ironePc = 
		Cast<AIrone3DPlayerController>(pc);
	ensure(ironePc);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	// during any transition, the player is invulnerable!
	player->setInvulnerable(true);
    /*
		1 ) Get the FString of the current Level name from GameState
        2 ) pause the game
		3 ) make the player able to move while paused
		4 ) set the player camera to be a certain static location for the transition
		5 ) set the player at a start location at the entrance to the transition
		6 ) tell the player to move to a location 
			that is out of view of the transition cam
		7 ) Get the FString of the next Level name from GameState, according to exitVec
        8 ) fade-out to black
            --- ON FADE-OUT COMPLETE ---
		9 ) set the new room's "hasBeenVisited" flag
        10) Load the destination level
            --- ON DESTINATION LEVEL LOADED ---
		11) if this is the first time the room has been loaded,
			cull enemies to fit the difficulty
        12) set the player camera to be a certain static location that will
			see the player when they finish moving into the new room
        13) set the room we came from to be not visible.  It can remain loaded
			since all the entities should now be paused
		14) unload the previous room
            --- ON SOURCE LEVEL UNLOADED ---
		15) move the player to a position that is out of the view of the new camera
		16) tell the player to move into a position that is in front of the 
			opposing RoomTransitionTrigger in the new Room
        17) fade-in from black
            --- ON FADE-IN COMPLETE ---
        18) unpause all the entities who were spawned from the current room
		19) make sure the player can no longer move while the game is paused
    */
	// destroy all projectiles
	for (TObjectIterator<AActor> actIt; actIt; ++actIt)
	{
		if (actIt->GetWorld() != world ||
			!actIt->IsA(ATurretLaser::StaticClass()))
		{
			continue;
		}
		actIt->Destroy();
	}
	//1 ) Get the FString of the current Level name from GameState
	//strTransitionLevelCameFrom = UGameplayStatics::GetCurrentLevelName(world);
	strTransitionLevelCameFrom = gs->currentRoomLevelName();
	///GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	///	FString::Printf(TEXT("strTransitionLevelCameFrom=%s"), *strTransitionLevelCameFrom));
	//2 ) pause the game
	//3 ) make the player able to move while paused
	TArray<TWeakObjectPtr<AActor>> currRoomActorSet = 
		gs->getCurrentRoomActorSet();
	for (auto& actor : currRoomActorSet)
	{
		if (!actor.IsValid())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("actor is invalid, skipping..."))
				continue;
		}
		UE_LOG(LogTemp, Warning,
			TEXT("setting CustomTimeDilation=0.f for actIt->GetName()=%s"),
			*actor->GetName());
		actor->CustomTimeDilation = 0.f;
	}
	//4 ) set the player camera to be a certain static location for the transition
	// -get the RoomTransitionTrigger the player is touching right now //
	check(trigger);
	// -determine which location the camera actor should go relative to the //
	//		trigger based on transitionExitDirection						//
	const FVector triggerLocation = trigger->GetActorLocation();
	// default location for east-facing transition triggers
	exitVec = trigger->getExitVector();
	const float exitVecRadians = FMath::Atan2(exitVec.Y, exitVec.X);
	FVector transitionCamOffset = TRANSITION_CAM_OFFSET.RotateAngleAxis(
		FMath::RadiansToDegrees(exitVecRadians), { 0,0,1 });
	UE_LOG(LogTemp, Warning, TEXT("transitionCamOffset=%s"),
		*transitionCamOffset.ToString());
	// -place a persistent camera actor at this location //
	transitionCamera->SetActorLocation(triggerLocation + transitionCamOffset);
	// -set the player's camera to be this persistent camera //
	pc->SetViewTarget(transitionCamera);
	//5 ) set the player at a start location at the entrance to the transition
	static const float TRANSITION_PLAYER_START_DISTANCE = -300;
	static const float TRANSITION_PLAYER_END_DISTANCE = 100;
	static const float TRANSITION_CAM_FOCUS_DISTANCE = -175;
	const FVector triggerBottom = trigger->getBottomLocation();
	pPawn->SetActorLocation(triggerLocation +
		exitVec * TRANSITION_PLAYER_START_DISTANCE);
	player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	const FVector camFocusLoc = triggerLocation +
		exitVec * TRANSITION_CAM_FOCUS_DISTANCE;
	const FVector playerToCamVec = 
		camFocusLoc - transitionCamera->GetActorLocation();
	transitionCamera->SetActorRotation(playerToCamVec.ToOrientationRotator());
	//6 ) tell the player to move to a location 
	//	that is out of view of the transition cam
	ironePc->transitionExitToLocation(trigger->GetActorLocation() + exitVec * TRANSITION_PLAYER_END_DISTANCE);
	//7 ) Get the FString of the next Level name from GameState, according to exitVec
	strTransitionLevelGoingTo = gs->adjacentRoomLevelName(exitVec);
	///GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	///	FString::Printf(TEXT("strTransitionLevelGoingTo=%s"), *strTransitionLevelGoingTo));
    //8 ) fade-out to black
	fadeOut(FLinearColor{ 0,0,0 }, LEVEL_TRANSITION_FADE_TIME);
	transitioning = true;
}
void Airone3dGameMode::startLevelTransition(ALevelTransitionTrigger* trigger)
{
	if (transitioning)
	{
		return;
	}
	UWorld*const world = GetWorld();
	ensure(world);
	/// ASSUMPTION: only one player per game:
	APlayerController*const pc = world->GetFirstPlayerController();
	ensure(pc);
	APawn*const pPawn = pc->GetPawn();
	ensure(pPawn);
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(pPawn);
	ensure(player);
	AIrone3DPlayerController*const ironePc = Cast<AIrone3DPlayerController>(pc);
	ensure(ironePc);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	// during any transition, the player is invulnerable!
	player->setInvulnerable(true);
	// -place a persistent camera actor at this location //
	player->copyCameraPropertiesTo(transitionCamera);
	// -set the player's camera to be this persistent camera //
	pc->SetViewTarget(transitionCamera);
	// save the player's inventory in the GameInstance so we can have it next
	//	level! //
	UIrone3dGameInstance*const gi =
		Cast<UIrone3dGameInstance>(GetGameInstance());
	ensure(gi);
	gi->setItemCounts(gs->getInventory()->getItemCounts());
	// transition us to the next level (hopefully!)
	transitioning = true;
	levelComplete = true;
	fadeOut(FLinearColor{ 0,0,0 }, LEVEL_TRANSITION_FADE_TIME);
}
void Airone3dGameMode::fadeOut(const FLinearColor & fadeColor, float fadeTime)
{
	fade(false, fadeColor, fadeTime);
}
void Airone3dGameMode::fadeIn(const FLinearColor & fadeColor, float fadeTime)
{
	fade(true, fadeColor, fadeTime);
}
void Airone3dGameMode::onLoadStreamLevelFinished()
{
	UWorld* world = GetWorld();
	if (!ensure(world)) return;
	/// ASSUMPTION: only one player per game:
	APlayerController* pc = world->GetFirstPlayerController();
	if (!ensure(pc)) return;
	AIrone3DPlayerController* ironePc = Cast<AIrone3DPlayerController>(pc);
	if (!ensure(ironePc)) return;
	APawn* pPawn = pc->GetPawn();
	if (!ensure(pPawn)) return;
	AIrone3DPlayer* player = Cast<AIrone3DPlayer>(pPawn);
	if (!ensure(player)) return;
	AIrone3dGameState* gs = world->GetGameState<AIrone3dGameState>();
	if (!ensure(gs)) return;
	// Need to move the persistent ANavMeshBoundsVolume into the center of the new Level //
	//	-First, find the ANavMeshBoundsVolume in the list of world Actors
	ANavMeshBoundsVolume* navMesh = nullptr;
	for (TObjectIterator<AActor> actIt; actIt; ++actIt)
	{
		if (actIt->GetWorld() != world)
		{
			continue;
		}
		auto navCast = Cast<ANavMeshBoundsVolume>(*actIt);
		if (navCast)
		{
			navMesh = navCast;
			break;
		}
	}
	if (navMesh)
	{
		//	-Get the world-offset location of the current room
		const FVector roomWorldOffset = gs->currentRoomWorldOffset();
		//	-set the ANavMeshBoundsVolume to be this new location
		FVector navMeshLocation = navMesh->GetActorLocation();
		navMeshLocation.X = roomWorldOffset.X;
		navMeshLocation.Y = roomWorldOffset.Y;
		navMesh->SetActorLocation(navMeshLocation);
	}
	// This line of code is retarded and there really should be a 
	//	better way up rebuilding nav meshes in game, especially since
	//	there is literally an option to have them rebuild dynamically
	//	in the editor, but here we are in reality...
	///pc->ConsoleCommand(TEXT("RebuildNavigation"));
	// So, none of this shit is documented anywhere, and the internet is
	//	full of forum posts of people trying to figure out how to do this,
	//	but getting no answers from devs on how to actually do it,
	//	sOOoo I stepped through the console command for RebuildNavigation
	//	and came across this function:
	// void UCheatManager::RebuildNavigation()
	UNavigationSystemV1* const navSys = 
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(world);
	if (navSys)
	{
		navSys->Build();
	}
	else
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Failed to get current navigation system!"));
	}
	/// DEBUG //////////////////////////////////////
	///for (auto& actor : currRoomActorSet)
	///{
	///	UE_LOG(LogTemp, Warning,
	///		TEXT("level stream finished loading! - actIt->GetName()=%s"),
	///		*actor->GetName())
	///}
	/// ////////////////////////////////////////////
	//11) if this is the first time the room has been loaded,
	//	cull enemies to fit the difficulty
	TArray<TWeakObjectPtr<AActor>> currRoomActorSet = 
		gs->getCurrentRoomActorSet();
	if (justLoadedRoom)
	{
		// initialize the time dilation to 0 when first loading a room to 
		//	prevent enemies from preemptively attacking the player
		for (auto& actor : currRoomActorSet)
		{
			if (!actor.IsValid())
			{
				UE_LOG(LogTemp, Warning,
					TEXT("actor is invalid, skipping..."))
					continue;
			}
			UE_LOG(LogTemp, Warning,
				TEXT("initializing CustomTimeDilation=0.f for actIt->GetName()=%s"),
				*actor->GetName());
			actor->CustomTimeDilation = 0.f;
		}
		///TODO: cull enemies to fit difficulty
	}
    //12) set the player camera to be a certain static location that will
	//	see the player when they finish moving into the new room
	//- find the ARoomTransitionTrigger that the player is moving through to get into the room
	//	we can do this by finding the ARoomTransitionTrigger that has the exitVec with the smallest
	//	dot product from exitVec, should yield the opposite exit direction...
	float smallestDotProd = 9999.f;
	ARoomTransitionTrigger* triggerEntrance = nullptr;
	for (auto& actor : currRoomActorSet)
	{
		if (!actor.IsValid())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("actor is invalid, skipping..."))
				continue;
		}
		// set all this room's actors to be visible again!
		actor->SetActorHiddenInGame(false);
		auto checkTrigger = Cast<ARoomTransitionTrigger>(actor.Get());
		if (!checkTrigger)
		{
			continue;
		}
		if (!triggerEntrance)
		{
			triggerEntrance = checkTrigger;
			const FVector checkVec = checkTrigger->getExitVector();
			smallestDotProd = FVector::DotProduct(checkVec, exitVec);
		}
		else
		{
			const FVector checkVec = checkTrigger->getExitVector();
			const float dotProd = FVector::DotProduct(checkVec, exitVec);
			//UE_LOG(LogTemp, Warning,
			//	TEXT("actIt->GetName()=%s dotProd=%f"),
			//	*actor->GetName(), dotProd);
			if (dotProd < smallestDotProd)
			{
				//UE_LOG(LogTemp, Warning,
				//	TEXT("dotProd is smaller than smallestDotProd=%f!!!"),
				//	smallestDotProd);
				smallestDotProd = dotProd;
				triggerEntrance = checkTrigger;
			}
		}
	}
	check(triggerEntrance);
	// -determine which location the camera actor should go relative to the //
	//		trigger based on transitionExitDirection						//
	const FVector triggerLocation = triggerEntrance->GetActorLocation();
	// default location for east-facing transition triggers
	const float exitVecRadians = FMath::Atan2(exitVec.Y, exitVec.X);
	FVector transitionCamOffset = TRANSITION_CAM_OFFSET.RotateAngleAxis(
		FMath::RadiansToDegrees(exitVecRadians) + 180, { 0,0,1 });
	UE_LOG(LogTemp, Warning, TEXT("entrance transitionCamOffset=%s"),
		*transitionCamOffset.ToString());
	// -place a persistent camera actor at this location //
	transitionCamera->SetActorLocation(triggerLocation + transitionCamOffset);
	// No need to set the player's camera to the transitionCamera, as it already is!
	// -set the player at a start location at the entrance to the transition
	static const float TRANSITION_PLAYER_START_DISTANCE = -100;
	static const float TRANSITION_PLAYER_END_DISTANCE = 140;
	static const float TRANSITION_CAM_FOCUS_DISTANCE = 175;
	const FVector triggerBottom = triggerEntrance->getBottomLocation();
	//15) move the player to a position that is out of the view of the new camera
	pPawn->SetActorLocation(triggerLocation +
		exitVec * TRANSITION_PLAYER_START_DISTANCE);
	player->GetCharacterMovement()->Velocity = FVector::ZeroVector;
	const FVector camFocusLoc = triggerLocation +
		exitVec * TRANSITION_CAM_FOCUS_DISTANCE;
	const FVector playerToCamVec =
		camFocusLoc - transitionCamera->GetActorLocation();
	transitionCamera->SetActorRotation(playerToCamVec.ToOrientationRotator());
	//16) tell the player to move into a position that is in front of the 
	//	opposing RoomTransitionTrigger in the new Room
	//	that is inside the new room and not colliding with the adjacent triggerEntrance
	ironePc->transitionExitToLocation(triggerEntrance->GetActorLocation() + exitVec * TRANSITION_PLAYER_END_DISTANCE);
    //13) set the room we came from to be not visible.  It can remain loaded
	//	since all the entities should now be paused
	//14) unload the previous room
	ULevelStreaming* levelStreaming = UGameplayStatics::GetStreamingLevel(
		world, FName(*strTransitionLevelCameFrom));
	check(levelStreaming);
	levelStreaming->SetShouldBeVisible(false);
    //17) fade-in from black
	fadeIn(FLinearColor{ 0,0,0 }, LEVEL_TRANSITION_FADE_TIME);
}
void Airone3dGameMode::onEndTransition()
{
	UWorld*const world = GetWorld();
	if (!ensure(world)) return;
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	if (!ensure(gs)) return;
	/// ASSUMPTION: only one player per game:
	APlayerController*const pc = world->GetFirstPlayerController();
	ensure(pc);
	APawn*const pPawn = pc->GetPawn();
	ensure(pPawn);
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(pPawn);
	ensure(player);
	// move player down onto the ground??
	player->SetActorLocation(player->GetActorLocation() +
		player->GetActorUpVector()*-100, true);
	// restore mortality to the player after they gain control again:
	player->setInvulnerable(false);
	//18) unpause all the entities who were spawned from the current room
	//19) make sure the player can no longer move while the game is paused
	TArray<TWeakObjectPtr<AActor>> currRoomActorSet = 
		gs->getCurrentRoomActorSet();
	for (auto& actor : currRoomActorSet)
	{
		if (!actor.IsValid())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("actor is invalid, skipping..."))
				continue;
		}
		UE_LOG(LogTemp, Warning,
			TEXT("setting CustomTimeDilation=1.f for actIt->GetName()=%s"),
			*actor->GetName());
		actor->CustomTimeDilation = 1.f;
	}
	transitioning = false;
}
bool Airone3dGameMode::isTransitioning() const
{
	return transitioning;
}
void Airone3dGameMode::BeginPlay()
{
	Super::BeginPlay();
}
void Airone3dGameMode::fade(bool in, const FLinearColor & fadeColor, float fadeTime)
{
	fadingIn = in;
	this->fadeTime = fadeTimeTotal = fadeTime;
	colorFullScreenTexture = fadeColor;
	colorFullScreenTexture.A = in ? 1.f : 0.f;
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Red,
	//	FString::Printf(TEXT("colorFullScreenTexture.A=%f"), colorFullScreenTexture.A));
	setFullscreenTextureColor(colorFullScreenTexture);
}
