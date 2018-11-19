// YOLO SWAG 420
#include "creep.h"
#include "AiControllerCreep.h"
#include "Irone3DPlayer.h"
#include "UnitComponent.h"
#include "Irone3dGameState.h"
#include <GameFramework/PawnMovementComponent.h>
#include <Runtime/AIModule/Classes/Perception/PawnSensingComponent.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystem.h>
#include <Runtime/Engine/Classes/Particles/ParticleSystemComponent.h>
const FLinearColor Acreep::HURT_OUTLINE_COLOR = {1.f,0.f,0.f,1.f};
const float Acreep::HURT_FLASH_SECONDS = 5.f;
Acreep::Acreep()
    :hurtFlashSeconds(0.f)
	,pawnSense(CreateDefaultSubobject<UPawnSensingComponent>(TEXT("pawnSense")))
	,unitComponent(CreateDefaultSubobject<UUnitComponent>(TEXT("unitComponent")))
{
	PrimaryActorTick.bCanEverTick = true;
}
float Acreep::lateralSpeed() const
{
    auto movementComp = GetMovementComponent();
    check(movementComp);
    const float scalarProjection = FVector::DotProduct(movementComp->Velocity, GetActorUpVector());
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
void Acreep::BeginPlay()
{
	Super::BeginPlay();
	unitComponent->addVulnerablePrimitiveComponent(GetCapsuleComponent());
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
	//TArray < TArray < FString > > sys;
	//TArray < TArray < FString > > param;
	//particleSystemBlood->GetParametersUtilized(sys, param);
	UWorld* world = GetWorld();
	if (!world)
	{
		return;
	}
	AIrone3dGameState* gs = world->GetGameState<AIrone3dGameState>();
	if (!gs)
	{
		return;
	}
	gs->addActorToCurrentRoom(this);
}
void Acreep::onSeePawn(APawn * pawn)
{
    auto controller = GetController();
    if (!controller)
    {
        return;
    }
    auto creepController = Cast<AAiControllerCreep>(controller);
    if (!creepController)
    {
        return;
    }
    auto player = Cast<AIrone3DPlayer>(pawn);
    if (!player)
    {
        return;
    }
    creepController->onSeeEnemyPawn(pawn);
}
void Acreep::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
void Acreep::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
float Acreep::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, 
	AController * EventInstigator, AActor * DamageCauser)
{
	hurtFlashSeconds = HURT_FLASH_SECONDS;
	auto world = GetWorld();
	if (world)
	{
		auto particleComp =
			UGameplayStatics::SpawnEmitterAtLocation(world, particleSystemBlood,
				GetActorLocation());
		particleComp->SetVectorParameter("color", FVector{ 1.f,0.f,0.f });
	}
	auto controller = GetController();
	APawn* instigatingPawn = nullptr;
	if (EventInstigator)
	{
		instigatingPawn = EventInstigator->GetPawn();
	}
	//UE_LOG(LogTemp, Warning, 
	//	TEXT("Is it even POSSIBLE? EventInstigator=%p EventInstigator->GetOwner()=%p DamageCauser=%p"),
	//	(void*)(EventInstigator), (void*)(instigatingActor), (void*)(DamageCauser));
	//if (EventInstigator)
	//{
	//	UE_LOG(LogTemp, Warning, 
	//		TEXT("\tEventInstigator=%s"),
	//		*EventInstigator->GetFName().ToString());
	//}
	//if (DamageCauser)
	//{
	//	UE_LOG(LogTemp, Warning, 
	//		TEXT("\tDamageCauser=%s"),
	//		*DamageCauser->GetFName().ToString());
	//}
	if (controller && instigatingPawn)
	{
		//UE_LOG(LogTemp, Warning, TEXT("YES"));
		auto creepController = Cast<AAiControllerCreep>(controller);
		if (creepController)
		{
			//UE_LOG(LogTemp, Warning, TEXT("IT IS"));
			creepController->aggro(instigatingPawn);
		}
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}
