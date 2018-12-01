// YOLO SWAG 420
#include "LevelTransitionTrigger.h"
#include <Engine/Classes/Components/BoxComponent.h>
#include "Irone3DPlayer.h"
#include "Irone3dGameState.h"
#include "irone3dGameMode.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <Runtime/Engine/Classes/GameFramework/GameModeBase.h>
ALevelTransitionTrigger::ALevelTransitionTrigger()
	:componentBox(CreateDefaultSubobject<UBoxComponent>(TEXT("box")))
{
	RootComponent = componentBox;
	PrimaryActorTick.bCanEverTick = true;
}
void ALevelTransitionTrigger::BeginPlay()
{
	Super::BeginPlay();
	componentBox->OnComponentBeginOverlap.AddDynamic(
		this, &ALevelTransitionTrigger::onBoxBeginOverlap);
	UWorld const *const world = GetWorld();
	ensure(world);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	if (gs)
	{
		gs->addActorToCurrentRoom(this);
	}
}
void ALevelTransitionTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ALevelTransitionTrigger::onBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep,
	const FHitResult & SweepResult)
{
	AIrone3DPlayer const*const player = Cast<AIrone3DPlayer>(OtherActor);
	if (!player)
	{
		return;
	}
	UWorld* world = GetWorld();
	ensure(world);
	AGameModeBase*const gmb = world->GetAuthGameMode();
	if (gmb)
	{
		Airone3dGameMode*const gm = Cast<Airone3dGameMode>(gmb);
		if (gm)
		{
			gm->startLevelTransition(this);
		}
	}
	///GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
	///	FString::Printf(TEXT("Level Trigger hit!!!")));
}
