// YOLO SWAG 420
#include "Item.h"
#include "Irone3DPlayer.h"
#include "Irone3dGameState.h"
#include "Inventory.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include <Runtime/Engine/Classes/Engine/World.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <Sound/SoundCue.h>
#include <Kismet/GameplayStatics.h>
#include <Particles/ParticleSystemComponent.h>
#include <Particles/ParticleSystem.h>
#include <ParticleEmitterInstances.h>
AItem::AItem()
	:capsuleComponent(CreateDefaultSubobject<UCapsuleComponent>(TEXT("capsule")))
	,meshComponent(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh")))
	,sparkleParticleComponent(CreateDefaultSubobject<UParticleSystemComponent>(TEXT("sparkle")))
{
	PrimaryActorTick.bCanEverTick = true;
	capsuleComponent->SetupAttachment(RootComponent);
	meshComponent->SetupAttachment(capsuleComponent);
}
void AItem::Tick(float DeltaTime)
{
	// This is the dumbest thing in the world, but apparently you just have to
	//	do this in order to get the particle system component rendering in the
	//	correct location...
	for (auto emitter : sparkleParticleComponent->EmitterInstances)
	{
		emitter->Location = GetActorLocation();
	}
	//sparkleParticleComponent->EmitterInstances[0]->Location
	///sparkleParticleComponent->SetRelativeLocation(FVector::ZeroVector);
	///sparkleParticleComponent->Activate();
	//sparkleParticleComponent->SetRelativeLocation(FVector::ZeroVector);
	static const float MESH_ROTATION_SPEED = 90;
	const float rotSpeed = MESH_ROTATION_SPEED*DeltaTime;
	meshComponent->AddRelativeRotation(FRotator(0.f, rotSpeed, 0.f));
	//meshComponent->SetCollisionProfileName("Default");
	Super::Tick(DeltaTime);
}
void AItem::setMeta(const FItemMeta & newMeta)
{
	itemMeta = newMeta;
	switch (itemMeta.type)
	{
	case ItemType::HEART:
		meshComponent->SetStaticMesh(meshHeart);
		break;
	case ItemType::SWORD:
		meshComponent->SetStaticMesh(meshSword);
		break;
	case ItemType::BOOT:
		meshComponent->SetStaticMesh(meshBoot);
		break;
	case ItemType::KEY:
		meshComponent->SetStaticMesh(meshKey);
		break;
	}
}
void AItem::BeginPlay()
{
	Super::BeginPlay();
	sparkleParticleComponent->SetRelativeLocation(FVector::ZeroVector);
	FAttachmentTransformRules attachRules(EAttachmentRule::SnapToTarget, true);
	sparkleParticleComponent->AttachToComponent(RootComponent, attachRules);
	sparkleParticleComponent->Activate(true);
	setMeta(itemMeta);
	if (!capsuleComponent->OnComponentBeginOverlap.IsAlreadyBound(
		this, &AItem::onOverlapBegin))
	{
		capsuleComponent->OnComponentBeginOverlap.AddDynamic(
			this, &AItem::onOverlapBegin);
	}
	UWorld const*const world = GetWorld();
	ensure(world);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	gs->addActorToCurrentRoom(this);
}
void AItem::onOverlapBegin(UPrimitiveComponent * OverlappedComponent,
	AActor * OtherActor, UPrimitiveComponent * OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UWorld*const world = GetWorld();
	ensure(world);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	if(OtherComp->IsA(UCapsuleComponent::StaticClass()))
	{
		UGameplayStatics::PlaySoundAtLocation(world, sfxItemPickup, GetActorLocation());
		gs->getInventory()->addItem(itemMeta.type);
		UGameplayStatics::SpawnEmitterAtLocation(world, sparkleBurst,
			GetActorLocation());
		Destroy();
	}
}
