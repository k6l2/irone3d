// YOLO SWAG 420
#include "creep.h"
#include "AiControllerCreep.h"
#include "Irone3DPlayer.h"
#include "UnitComponent.h"
#include "Irone3dGameState.h"
#include <GameFramework/PawnMovementComponent.h>
#include <Runtime/AIModule/Classes/Perception/PawnSensingComponent.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <Runtime/Engine/Classes/Components/SphereComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystem.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystemComponent.h>
#include "CombatComponent.h"
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>
#include <GameFramework/CharacterMovementComponent.h>
const FLinearColor Acreep::HURT_OUTLINE_COLOR = {1.f,0.f,0.f,1.f};
const float Acreep::HURT_FLASH_SECONDS = 5.f;
Acreep::Acreep()
    :hurtFlashSeconds(0.f)
	,pawnSense(CreateDefaultSubobject<UPawnSensingComponent>(TEXT("pawnSense")))
	,unitComponent(CreateDefaultSubobject<UUnitComponent>(TEXT("unitComponent")))
	,componentAttackSphere(CreateDefaultSubobject<USphereComponent>(TEXT("attackSphere")))
	,componentAggroSphere(CreateDefaultSubobject<USphereComponent>(TEXT("aggroSphere")))
	,componentCombat(CreateDefaultSubobject<UCombatComponent>(TEXT("combat")))
{
	componentAttackSphere->SetupAttachment(RootComponent);
	componentCombat->SetupAttachment(componentAttackSphere);
	componentCombat->setAlwaysAttacking(true);
	componentAggroSphere->SetupAttachment(RootComponent);
	PrimaryActorTick.bCanEverTick = true;
}
float Acreep::lateralSpeed() const
{
    auto movementComp = GetMovementComponent();
	if (!ensure(movementComp)) return 0.f;
    const float scalarProjection = 
		FVector::DotProduct(movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    const FVector lateralVelocity = movementComp->Velocity - upVelocity;
    return lateralVelocity.Size();
}
bool Acreep::canSee(AActor* actor) const
{
    if (!pawnSense)
    {
        return false;
    }
    return pawnSense->HasLineOfSightTo(actor);
}
void Acreep::getOverlappingAggroActors(TArray<AActor*>& outArray) const
{
	if (!ensure(componentAggroSphere))
	{
		return;
	}
	componentAggroSphere->GetOverlappingActors(outArray);
}
float Acreep::getPushAwaySeconds() const
{
	return pushAwaySeconds;
}
void Acreep::BeginPlay()
{
	Super::BeginPlay();
	if (ensure(unitComponent))
	{
		unitComponent->delegateDie.BindUFunction(this, "onUnitDie");
		unitComponent->addVulnerablePrimitiveComponent(GetCapsuleComponent());
	}
	if (pawnSense)
    {
		if (!pawnSense->OnSeePawn.IsAlreadyBound(this, &Acreep::onSeePawn))
		{
			pawnSense->OnSeePawn.AddDynamic(this, &Acreep::onSeePawn);
		}
    }
	const int32 outlineMaterialIndex = GetMesh()->GetMaterialIndex("materialOutline");
	const int32 eyesMaterialIndex = GetMesh()->GetMaterialIndex("materialEyes");
	outlineMaterial = 
		GetMesh()->CreateAndSetMaterialInstanceDynamic(outlineMaterialIndex);
	eyesMaterial = 
		GetMesh()->CreateAndSetMaterialInstanceDynamic(eyesMaterialIndex);
	FMaterialParameterInfo outlineColorInfo;
	outlineColorInfo.Name = "color";
	outlineMaterial->GetVectorParameterValue(outlineColorInfo, defaultOutlineColor);
	UWorld const*const world = GetWorld();
	if (!ensure(world)) return;
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	if (!ensure(gs)) return;
	gs->addActorToCurrentRoom(this);
}
void Acreep::onSeePawn(APawn * pawn)
{
    AController*const controller = GetController();
	if (!ensure(controller)) return;
	AAiControllerCreep*const creepController = 
		Cast<AAiControllerCreep>(controller);
	if (!ensure(creepController)) return;
	AIrone3DPlayer const*const player = Cast<AIrone3DPlayer>(pawn);
	if (player)
	{
		creepController->onSeeEnemyPawn(pawn);
	}
}
void Acreep::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (pushAwaySeconds > 0)
	{
		pushAwaySeconds -= DeltaTime;
	}
	const float hurtPercent = 
		FMath::Clamp(hurtFlashSeconds / HURT_FLASH_SECONDS, 0.f,1.f);
	if (hurtFlashSeconds)
	{
		hurtFlashSeconds -= DeltaTime;
	}
	const float colorLerpProgress = FMath::Pow(hurtPercent, 2);
	const FLinearColor outlineColor = FLinearColor::LerpUsingHSV(
		defaultOutlineColor, HURT_OUTLINE_COLOR, colorLerpProgress);
	outlineMaterial->SetVectorParameterValue("color", outlineColor);
	eyesMaterial->SetVectorParameterValue("color", outlineColor);
}
float Acreep::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, 
	AController * EventInstigator, AActor * DamageCauser)
{
	hurtFlashSeconds = HURT_FLASH_SECONDS;
	UWorld const*const world = GetWorld();
	if (world)
	{
		if (!unitComponent->isDead())
		{
			UGameplayStatics::PlaySoundAtLocation(world, sfxHit, GetActorLocation());
		}
		auto particleComp =
			UGameplayStatics::SpawnEmitterAtLocation(world, particleSystemBlood,
				GetActorLocation());
		particleComp->SetVectorParameter("color", FVector{ 1.f,0.f,0.f });
	}
	AController*const controller = GetController();
	AAiControllerCreep*const creepController = 
		Cast<AAiControllerCreep>(controller);
	APawn* instigatingPawn = nullptr;
	if (EventInstigator)
	{
		instigatingPawn = EventInstigator->GetPawn();
		AIrone3DPlayer*const instigatingPlayer = 
			Cast<AIrone3DPlayer>(instigatingPawn);
		UCharacterMovementComponent*const charMoveComp =
			Cast< UCharacterMovementComponent>(GetMovementComponent());
		if (instigatingPlayer && charMoveComp && ensure(creepController))
		{
			const FVector prevVelocity = charMoveComp->Velocity;
			// if we're hit by the player, we want the creep to move away from
			//	the player a bit just like original Irone
			pushAwaySeconds = 1.5f;
			// need to call stopmovement to end the pathfinding procs that are
			//	happening in the background somewhere in the engine's pathfind
			//	logic
			creepController->StopMovement();
			const FVector awayFromPlayerVec =
				(GetActorLocation() - instigatingPlayer->GetActorLocation()).
					GetSafeNormal2D();
			///UE_LOG(LogTemp, Warning, TEXT("charMoveComp->Velocity=%s awayFromPlayerVec=%s"), 
			///	*charMoveComp->Velocity.ToString(), *awayFromPlayerVec.ToString());
			///charMoveComp->AddForce(awayFromPlayerVec * -100000000);
			if (FVector::DotProduct(
				prevVelocity, awayFromPlayerVec) < 0)
			{
				///UE_LOG(LogTemp, Warning, TEXT("BOOST!!!"));
				charMoveComp->Velocity = prevVelocity * -0.5f;
				///FRotator rotation = GetActorRotation();
				///rotation.Yaw = awayFromPlayerVec.Rotation().Yaw;
				///SetActorRotation(rotation);
				SetActorRotation(awayFromPlayerVec.Rotation());
			}
			//charMoveComp->Velocity = awayFromPlayerVec * 500;
			//FRotator rotation = GetActorRotation();
			//rotation.Yaw = awayFromPlayerVec.Rotation().Yaw;
			//SetActorRotation(rotation);
		}
	}
	if (ensure(creepController) && instigatingPawn)
	{
		creepController->aggro(instigatingPawn);
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
void Acreep::onUnitDie()
{
	UWorld const*const world = GetWorld();
	if (world)
	{
		UGameplayStatics::PlaySoundAtLocation(
			world, sfxDestroyed, GetActorLocation());
	}
	Destroy();
}
