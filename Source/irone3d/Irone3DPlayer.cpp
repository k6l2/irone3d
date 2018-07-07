// YOLO SWAG 420
#include "Irone3DPlayer.h"
#include <Camera/CameraComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/PawnMovementComponent.h>
#include <GameFramework/CharacterMovementComponent.h>
#include <Runtime/Engine/Classes/Components/StaticMeshComponent.h>
#include <Runtime/Engine/Classes/Components/SkeletalMeshComponent.h>
#include <Runtime/Engine/Classes/Materials/MaterialInstanceDynamic.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include <EngineGlobals.h>
#include "CombatComponent.h"
#include "Irone3DPlayer.h"
#include "Irone3DPlayerController.h"
AIrone3DPlayer::AIrone3DPlayer()
    :cameraBoom(CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom")))
    ,camera(CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera")))
    ,meshCharacter(CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshCharacter")))
    ,meshAttack(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshAttack")))
    ,attackCombatComponent(CreateDefaultSubobject<UCombatComponent>(TEXT("AttackCombat")))
    ,hardLanding(false)
    ,canAttack(false)
    ,tryingAttack(false)
    ,attackAnimationPlaying(false)
    ,attackAnimationProgress(0.5f)
{
	PrimaryActorTick.bCanEverTick = true;
    cameraBoom->SetupAttachment(RootComponent);
    camera->SetupAttachment(cameraBoom, USpringArmComponent::SocketName);
	meshCharacter->SetupAttachment(RootComponent);
    meshAttack->SetupAttachment(RootComponent);
	attackCombatComponent->SetupAttachment(meshAttack);
}
void AIrone3DPlayer::moveForward(float value)
{
    if (!Controller || value == 0.0f || hardLanding)
    {
        return;
    }
	auto pController = Cast<AIrone3DPlayerController>(Controller);
	if (pController && pController->isAutoMoving())
	{
		AddMovementInput(GetActorRotation().Vector());
		return;
	}
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    // get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, value);
}
void AIrone3DPlayer::moveRight(float value)
{
    if (!Controller || value == 0.0f || hardLanding)
    {
        return;
    }
    // find out which way is forward
    const FRotator Rotation = Controller->GetControlRotation();
    const FRotator YawRotation(0, Rotation.Yaw, 0);
    // get forward vector
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(Direction, value);
}
void AIrone3DPlayer::turnRate(float value)
{
    auto world = GetWorld();
    if (!world)
    {
        return;
    }
    AddControllerYawInput(value * world->GetDeltaSeconds());
}
void AIrone3DPlayer::lookUpRate(float value)
{
    auto world = GetWorld();
    if (!world)
    {
        return;
    }
    AddControllerPitchInput(value * world->GetDeltaSeconds());
}
float AIrone3DPlayer::lateralSpeed() const
{
    auto movementComp = GetMovementComponent();
    check(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    const FVector lateralVelocity = movementComp->Velocity - upVelocity;
    return lateralVelocity.Size();
}
float AIrone3DPlayer::verticalSpeed() const
{
    auto movementComp = GetMovementComponent();
    check(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    return upVelocity.Size();
}
float AIrone3DPlayer::verticalVelocity1D() const
{
    auto movementComp = GetMovementComponent();
    check(movementComp);
    const float scalarProjection = FVector::DotProduct(
		movementComp->Velocity, GetActorUpVector());
    const FVector upVelocity = scalarProjection*GetActorUpVector();
    const float upDotUpVelocity = FVector::DotProduct(
		upVelocity, GetActorUpVector());
    return upVelocity.Size()*(upDotUpVelocity<0 ? -1:1);
}
bool AIrone3DPlayer::isFalling() const
{
	if (Controller)
	{
		auto pController = Cast<AIrone3DPlayerController>(Controller);
		if (pController && pController->isAutoMoving())
		{
			return false;
		}
	}
    auto movementComp = GetMovementComponent();
    check(movementComp);
    return movementComp->IsFalling();
}
void AIrone3DPlayer::hardLandingStart()
{
    hardLanding = true;
}
void AIrone3DPlayer::hardLandingEnd()
{
    hardLanding = false;
}
void AIrone3DPlayer::canAttackStart()
{
    canAttack = true;
}
void AIrone3DPlayer::canAttackEnd()
{
    canAttack = false;
}
void AIrone3DPlayer::tryAttackStart()
{
    tryingAttack = true;
}
void AIrone3DPlayer::tryAttackEnd()
{
    tryingAttack = false;
}
bool AIrone3DPlayer::isTryingToAttack() const
{
	//UE_LOG(LogTemp, Warning,
	//	TEXT("tryingAttack=%s"),
	//	(tryingAttack ? "true":"false"))
    return tryingAttack;
}
void AIrone3DPlayer::attackStart()
{
    attackAnimationPlaying = true;
	attackCombatComponent->startAttack();
}
void AIrone3DPlayer::attackEnd()
{
    attackAnimationPlaying = false;
	attackCombatComponent->stopAttack();
}
void AIrone3DPlayer::updateAttackAnimationProgress(float value)
{
    attackAnimationProgress = value;
}
void AIrone3DPlayer::BeginPlay()
{
	Super::BeginPlay();
    dynMaterialSlash = 
		meshAttack->CreateAndSetMaterialInstanceDynamicFromMaterial(
			0, meshAttack->GetMaterial(0));
}
void AIrone3DPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if (attackAnimationPlaying)
    {
        dynMaterialSlash->SetScalarParameterValue(
			"swipeOffset", attackAnimationProgress);
    }
    else
    {
        dynMaterialSlash->SetScalarParameterValue(
			"swipeOffset", 0.f);
    }
	// Player animation should be able to tick while the game is paused
	//	such as during transitions between rooms
	//meshCharacter->SetTickableWhenPaused(GetTickableWhenPaused());
}
void AIrone3DPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
