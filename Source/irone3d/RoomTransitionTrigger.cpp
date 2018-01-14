// YOLO SWAG 420
#include "RoomTransitionTrigger.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <Runtime/Engine/Classes/Components/ArrowComponent.h>
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/GameFramework/GameModeBase.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "irone3dGameMode.h"
#include "Irone3dGameState.h"
#include "Irone3DPlayer.h"
ARoomTransitionTrigger::ARoomTransitionTrigger()
	:boxComponent(CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent")))
	,exitDirection(CreateDefaultSubobject<UArrowComponent>(TEXT("arrowComponent")))
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(boxComponent);
	boxComponent->SetMobility(EComponentMobility::Static);
	boxComponent->OnComponentBeginOverlap.AddDynamic(this, &ARoomTransitionTrigger::onOverlapBegin);
	exitDirection->SetupAttachment(RootComponent);
	/// I don't think I actually need to do this...
	//exitDirection->SetRelativeLocation(FVector::ZeroVector);
}
FVector ARoomTransitionTrigger::getExitVector() const
{
	return exitDirection->GetForwardVector();
}
FVector ARoomTransitionTrigger::getBottomLocation() const
{
	const FVector boxExtent = boxComponent->GetScaledBoxExtent();
	return GetActorLocation() + FVector{0,0,-boxExtent.Z};
}
void ARoomTransitionTrigger::BeginPlay()
{
	UWorld* world = GetWorld();
	check(world);
	if (!world)
	{
		return;
	}
	AIrone3dGameState* gs = world->GetGameState<AIrone3dGameState>();
	check(gs);
	if (!gs)
	{
		return;
	}
	gs->addActorToCurrentRoom(this);
}
void ARoomTransitionTrigger::onOverlapBegin(UPrimitiveComponent * OverlappedComp,
	AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult & SweepResult)
{
	if (!Cast<AIrone3DPlayer>(OtherActor))
	{
		return;
	}
	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}
	AGameModeBase* gmb = world->GetAuthGameMode();
	if (!gmb)
	{
		return;
	}
	Airone3dGameMode* gm = Cast<Airone3dGameMode>(gmb);
	if (!gm)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
		FString::Printf(TEXT("TransitionTrigger hit!!!")));
	gm->startRoomTransition(this);
}
