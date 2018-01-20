// YOLO SWAG 420

#include "SpikeTrap.h"
#include "CombatComponent.h"
#include <Runtime/Engine/Classes/Components/BoxComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Irone3DPlayer.h"
// Sets default values
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
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASpikeTrap::BeginPlay()
{
	Super::BeginPlay();
	///if (!skeletalMeshComponent->OnComponentBeginOverlap.IsAlreadyBound(
	///	this, &ASpikeTrap::onOverlapBegin))
	///{
	///	skeletalMeshComponent->OnComponentBeginOverlap.AddDynamic(
	///		this, &ASpikeTrap::onOverlapBegin);
	///	skeletalMeshComponent->OnComponentEndOverlap.AddDynamic(
	///		this, &ASpikeTrap::onOverlapEnd);
	///}
}

///void ASpikeTrap::onOverlapBegin(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
///{
///	
///}
///
///void ASpikeTrap::onOverlapEnd(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
///{
///
///}

// Called every frame
void ASpikeTrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

