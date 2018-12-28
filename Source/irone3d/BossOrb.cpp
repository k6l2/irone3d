// YOLO SWAG 420
#include "BossOrb.h"
#include <Engine/Classes/GameFramework/Pawn.h>
#include <Runtime/Engine/Classes/Components/StaticMeshComponent.h>
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/CoreUObject/Public/UObject/ConstructorHelpers.h>
#include "CombatComponent.h"
#include "UnitComponent.h"
#include <Runtime/Engine/Classes/GameFramework/Controller.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <Particles/ParticleSystem.h>
ABossOrb::ABossOrb()
	: componentSphere(CreateDefaultSubobject<USphereComponent    >(TEXT("sphere")))
	, componentMesh  (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"  )))
	, componentCombat(CreateDefaultSubobject<UCombatComponent    >(TEXT("combat")))
	, componentUnit  (CreateDefaultSubobject<UUnitComponent      >(TEXT("unit"  )))
{
	RootComponent = componentSphere;
	componentMesh->SetupAttachment(RootComponent);
	componentCombat->SetupAttachment(componentMesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT(
		"StaticMesh'/Game/gfx/orb/orb.orb'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialAsset(TEXT(
		"MaterialInstanceConstant'/Game/gfx/items/MaterialOutline-heart.MaterialOutline-heart'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> soundCueHit(TEXT(
		"SoundCue'/Game/sfx/hit_Cue.hit_Cue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> soundCueReflect(TEXT(
		"SoundCue'/Game/sfx/magic_reflect_Cue.magic_reflect_Cue'"));
	static ConstructorHelpers::FObjectFinder<USoundCue> soundCueDestroy(TEXT(
		"SoundCue'/Game/sfx/destroyed_Cue.destroyed_Cue'"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> psDestroy(TEXT(
		"ParticleSystem'/Game/gfx/particles/psBlood.psBlood'"));
	if (ensure(meshAsset.Succeeded()))
	{
		componentMesh->SetStaticMesh(meshAsset.Object);
	}
	if (ensure(materialAsset.Succeeded()))
	{
		materialUnreflectable = UMaterialInstanceDynamic::Create(
			materialAsset.Object, materialAsset.Object);
	}
	if (ensure(soundCueHit.Succeeded()))
	{
		sfxHitWall = soundCueHit.Object;
	}
	if (ensure(soundCueReflect.Succeeded()))
	{
		sfxReflectAttack = soundCueReflect.Object;
	}
	if (ensure(soundCueDestroy.Succeeded()))
	{
		sfxDestroyed = soundCueDestroy.Object;
	}
	if (ensure(psDestroy.Succeeded()))
	{
		particleSystemExplode = psDestroy.Object;
	}
	componentSphere->SetCollisionProfileName("BossOrb");
	componentMesh->SetCollisionProfileName("OverlapOnlyPlayer");
	componentSphere->SetSphereRadius(150);
	bounceSeconds = 20;
	flyAwaySeconds = 5;
	componentUnit->setHitpoints(420);
	componentUnit->addVulnerablePrimitiveComponent(componentSphere);
	PrimaryActorTick.bCanEverTick = true;
}
void ABossOrb::BeginPlay()
{
	Super::BeginPlay();
	ensure(componentCombat);
	componentCombat->delegateDamageDealt.BindUFunction(this, "onDamageDealt");
	componentCombat->startAttack();
}
void ABossOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UWorld const*const world = GetWorld();
	if (bounceSeconds > 0)
	{
		bounceSeconds -= DeltaTime;
		if (bounceSeconds <= 0)
		{
			componentSphere->SetCollisionEnabled(
				ECollisionEnabled::Type::NoCollision);
		}
	}
	else if (flyAwaySeconds > 0)
	{
		flyAwaySeconds -= DeltaTime;
	}
	else
	{
		Destroy();
	}
	FHitResult hitResult;
	///falling = true;
	///if (gravityOn)
	///{
	///	static const float GRAVITY_ACCEL = 980.665;//cm/sec^2
	///	velocity += GetGravityDirection()*GRAVITY_ACCEL*DeltaTime;
	///}
	SetActorLocation(
		GetActorLocation() + (velocity * DeltaTime), true, &hitResult);
	if (hitResult.IsValidBlockingHit())
	{
		// Reflect the orb's velocity with respect to the impact normal! //
		///UE_LOG(LogTemp, Warning, TEXT("hitResult.ImpactNormal=%s"),
		///	*hitResult.ImpactNormal.ToString());
		velocity += -2 * (FVector::DotProduct(
			velocity, hitResult.ImpactNormal)*hitResult.ImpactNormal);
		if (world)
		{
			UGameplayStatics::PlaySoundAtLocation(world, sfxHitWall, GetActorLocation());
		}
	}
}
///void ABossOrb::setVelocity(const FVector& v)
///{
///	velocity = v;
///}
float ABossOrb::TakeDamage(float DamageAmount,
	FDamageEvent const& DamageEvent, AController * EventInstigator,
	AActor * DamageCauser)
{
	UWorld const*const world = GetWorld();
	if (isReflectable)
	{
		if (world)
		{
			UGameplayStatics::PlaySoundAtLocation(world, sfxReflectAttack, GetActorLocation());
		}
		componentMesh->SetCollisionProfileName("OverlapOnlyEnemy");
		bounceSeconds = 0;
		componentSphere->SetCollisionEnabled(
			ECollisionEnabled::Type::NoCollision);
		flyAwaySeconds = 5;
		// first, change the direction of velocity to be AWAY from the 
		//	DamageCauser //
		const FVector toTarget =
			GetActorLocation() - DamageCauser->GetActorLocation();
		velocity = velocity.Size() * toTarget.GetSafeNormal();
		// next, set the pitch of the velocity to be that of the vector towards
		//	the instigator //
		const APawn* const instigator = GetInstigator();
		ensure(instigator);
		const FVector toInstigator = 
			instigator->GetActorLocation() - GetActorLocation();
		const float degreesFromUpTarget = FMath::RadiansToDegrees(FMath::Acos(
			FVector::DotProduct(toTarget.GetSafeNormal(), 
				GetActorUpVector())));
		const float degreesFromUpInstigator = FMath::RadiansToDegrees(
			FMath::Acos( FVector::DotProduct(toInstigator.GetSafeNormal(), 
				GetActorUpVector())));
		const float deltaPitchDegrees = 
			degreesFromUpInstigator - degreesFromUpTarget;
		///UE_LOG(LogTemp, Warning, TEXT("deltaPitchDegrees=%f"), deltaPitchDegrees);
		velocity = velocity.RotateAngleAxis(deltaPitchDegrees,
			FVector::CrossProduct(GetActorUpVector(), 
				velocity.GetSafeNormal()));
	}
	return Super::TakeDamage(
		DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void ABossOrb::shootAt(APawn* targetPawn, bool reflectable)
{
	if (!ensure(targetPawn))
	{
		return;
	}
	static const float SHOOT_SPEED = 1500;
	const FVector toTarget = ( targetPawn->GetActorLocation() + 
		FVector(0,0,100) ) - GetActorLocation();
	velocity = SHOOT_SPEED*toTarget.GetSafeNormal();
	isReflectable = reflectable;
	if (!isReflectable)
	{
		componentMesh->SetMaterial(1, materialUnreflectable);
	}
}
void ABossOrb::onDamageDealt()
{
	UWorld const*const world = GetWorld();
	if (world)
	{
		UGameplayStatics::PlaySoundAtLocation(world, sfxDestroyed, GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(world, particleSystemExplode,
			GetActorLocation());
	}
	Destroy();
}
