// YOLO SWAG 420
#include "BreakableFloor.h"
#include "Irone3DPlayer.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <GameFramework/Character.h>
#include <Runtime/Core/Public/Containers/Array.h>
#include <Runtime/Engine/Classes/Engine/StaticMeshActor.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <DrawDebugHelpers.h>
#include <DestructibleActor.h>
#include <DestructibleComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/Engine/Classes/Components/DecalComponent.h>
ABreakableFloor::ABreakableFloor()
	: boxComponentRoot(CreateDefaultSubobject<UBoxComponent  >(TEXT("boxComponentRoot")))
	, decalComponent  (CreateDefaultSubobject<UDecalComponent>(TEXT("decalComponent")))
	, floorActor(nullptr)
	, integritySeconds(3.f)
	, decalMaterial(nullptr)
{
	RootComponent = boxComponentRoot;
	decalComponent->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}
void ABreakableFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (manualTrigger)
	{
		return;
	}
	///UWorld* const world = GetWorld();
	///if (!ensure(floorActor) || !ensure(world))
	///{
	///	/// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
	///	/// 	FString::Printf(TEXT("WTF")));
	///	return;
	///}
	TArray<AActor*> overlappingActors;
	boxComponentRoot->GetOverlappingActors(
		overlappingActors, TSubclassOf<ACharacter>{});
	/// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
	/// 	FString::Printf(TEXT("overlapping=%d"), overlappingActors.Num()));
	const FVector boxLocation     = GetActorLocation();
	const FVector scaledBoxExtent = boxComponentRoot->GetScaledBoxExtent();
	bool containsStandingPlayer = false;
	for (int32 i = 0; i < overlappingActors.Num(); i++)
	{
		const FVector playerLoc = overlappingActors[i]->GetActorLocation();
		auto player = Cast<AIrone3DPlayer>(overlappingActors[i]);
		if (player &&
			!player->isFalling() &&
			playerLoc.X >= boxLocation.X - scaledBoxExtent.X &&
			playerLoc.X <= boxLocation.X + scaledBoxExtent.X &&
			playerLoc.Y >= boxLocation.Y - scaledBoxExtent.Y &&
			playerLoc.Y <= boxLocation.Y + scaledBoxExtent.Y)
		{
			containsStandingPlayer = true;
			break;
		}
	}
	if (containsStandingPlayer && integritySeconds > 0.f)
	{
		integritySeconds -= DeltaTime;
	}
	if (integritySeconds > 2.95f)
	{
		decalMaterial->SetScalarParameterByIndex(frameParamIndex, 0.f);
	}
	else if (integritySeconds > 1.f)
	{
		decalMaterial->SetScalarParameterByIndex(frameParamIndex, 1.f);
	}
	else
	{
		decalMaterial->SetScalarParameterByIndex(frameParamIndex, 2.f);
	}
	if (integritySeconds <= 0.f)
	{
		trigger();
	}
}
void ABreakableFloor::trigger()
{
	UWorld* const world = GetWorld();
	if (!ensure(floorActor) || !ensure(world))
	{
		/// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red,
		/// 	FString::Printf(TEXT("WTF")));
		return;
	}
	// cause the destructible actor to break immediately //
	const FVector floorActorLocation = floorActor->GetActorLocation();
	floorActor->Destroy();
	Destroy();
	// Spawn a destructible actor at the 
	//	same position of the static mesh actor //
	FActorSpawnParameters spawnParams;
	spawnParams.bNoFail = true;
	ADestructibleActor* destructible =
		world->SpawnActor<ADestructibleActor>(destructibleTileClass,
			floorActorLocation, FRotator::ZeroRotator, spawnParams);
	if (destructible && destructible->IsA(destructibleTileClass))
	{
		/// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green,
		/// 	FString::Printf(TEXT("destructible spawned!")));
		destructible->GetDestructibleComponent()->ApplyRadiusDamage(
			100.f, destructible->GetActorLocation(), 250.f, 2500.f, true);
	}
}
void ABreakableFloor::BeginPlay()
{
	Super::BeginPlay();
	const UWorld* const world = GetWorld();
	check(world);
	if (!world)
	{
		return;
	}
	// obtain a reference to the  static mesh actor 
	//	of the floor tile directly below us //
	FHitResult hitResult;
	const FVector traceEnd = GetActorLocation() - GetActorUpVector()*1000.f;
	const bool hitSomething = world->LineTraceSingleByProfile(
		hitResult, GetActorLocation(), traceEnd, "BlockOnlyWorld");
	if (hitSomething)
	{
		AActor* hitActor = hitResult.Actor.Get();
		if (hitActor->IsA(AStaticMeshActor::StaticClass()))
		{
			floorActor = Cast<AStaticMeshActor>(hitResult.Actor.Get());
		}
	}
	if (!floorActor)
	{
		Destroy();
	}
	// Position us to be directly over the static mesh //
	FVector newLocation{ 0.f,0.f, GetActorLocation().Z };
	newLocation.X = floorActor->GetActorLocation().X;
	newLocation.Y = floorActor->GetActorLocation().Y;
	SetActorLocation(newLocation);
	// adjust the X-Y dimensions of the 
	//	box component to match the static mesh actor//
	FVector floorOrigin;
	FVector floorBoxExtent;
	floorActor->GetActorBounds(true, floorOrigin, floorBoxExtent);
	boxComponentRoot->SetBoxExtent(floorBoxExtent);
	// adjust the decal to match the box component //
	decalComponent->DecalSize.Y = floorBoxExtent.X;
	decalComponent->DecalSize.Z = floorBoxExtent.Y;
	// Create a dynamic material instance for the decal //
	decalMaterial = decalComponent->CreateDynamicMaterialInstance();
	decalMaterial->InitializeScalarParameterAndGetIndex(
		"frame", 0, frameParamIndex);
}
