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
ABossOrb::ABossOrb()
	: componentSphere(CreateDefaultSubobject<USphereComponent    >(TEXT("sphere")))
	, componentMesh  (CreateDefaultSubobject<UStaticMeshComponent>(TEXT("mesh"  )))
	, componentCombat(CreateDefaultSubobject<UCombatComponent    >(TEXT("combat")))
	, componentUnit  (CreateDefaultSubobject<UUnitComponent      >(TEXT("unit"  )))
{
	RootComponent = componentSphere;
	componentMesh->SetupAttachment(RootComponent);
	componentCombat->SetupAttachment(componentMesh);
	componentCombat->setDestroyOnDealDamage(true);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT(
		"StaticMesh'/Game/gfx/orb/orb.orb'"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> materialAsset(TEXT(
		"MaterialInstanceConstant'/Game/gfx/items/MaterialOutline-heart.MaterialOutline-heart'"));
	if (ensure(meshAsset.Succeeded()))
	{
		componentMesh->SetStaticMesh(meshAsset.Object);
	}
	if (ensure(materialAsset.Succeeded()))
	{
		materialUnreflectable = UMaterialInstanceDynamic::Create(
			materialAsset.Object, materialAsset.Object);
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
	componentCombat->startAttack();
}
void ABossOrb::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
	if (isReflectable)
	{
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
