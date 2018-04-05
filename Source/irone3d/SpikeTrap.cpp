// YOLO SWAG 420
#include "SpikeTrap.h"
#include "CombatComponent.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Irone3DPlayer.h"
ASpikeTrap::ASpikeTrap()
	:combatComponent(CreateDefaultSubobject<UCombatComponent>(TEXT("combatComponent")))
	,skeletalMeshComponent(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("skeletalMeshComponent")))
	,boxComponent(CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponent")))
	,boxComponentRoot(CreateDefaultSubobject<UBoxComponent>(TEXT("boxComponentRoot")))
	,sceneComponent(CreateDefaultSubobject<USceneComponent>(TEXT("sceneComponent")))
	,activationTimer(0.f)
{
	RootComponent = sceneComponent;
	skeletalMeshComponent->SetupAttachment(RootComponent);
	boxComponent->SetupAttachment(skeletalMeshComponent, "spikeBoneSocket");
	boxComponentRoot->SetupAttachment(skeletalMeshComponent);
	combatComponent->SetupAttachment(boxComponent);
	PrimaryActorTick.bCanEverTick = true;

}
bool ASpikeTrap::trapActivated() const
{
	TArray<AActor*> overlappingActors;
	boxComponentRoot->GetOverlappingActors(overlappingActors);
	for (int32 i = 0; i < overlappingActors.Num(); i++)
	{
		if (overlappingActors[i]->IsA(ASpikeTrap::StaticClass()))
		{
			overlappingActors.RemoveAt(i);
			i--;
		}
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//	FString::Printf(TEXT("trap activated????? overlapping=%d"), overlappingActors.Num()));
	return overlappingActors.Num() > 0;
}
void ASpikeTrap::startCombat() const
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
	//	FString::Printf(TEXT("Get Fucked!")));
	combatComponent->startAttack();
}
void ASpikeTrap::stopCombat() const
{
	combatComponent->stopAttack();
}
