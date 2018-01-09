// YOLO SWAG 420
#include "Irone3DPlayerController.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include "Irone3DPlayer.h"
#define ECC_PLAYER_PAWN ECC_GameTraceChannel1
#define ECC_ENEMY_PAWN  ECC_GameTraceChannel2
AIrone3DPlayerController::AIrone3DPlayerController()
    :baseTurnRate(45)
    ,baseLookUpRate(45)
    ,deadzone(0.01)
	,autoMoveToLocation(false)
{
}
void AIrone3DPlayerController::transitionExitToLocation(const FVector & exitLocation)
{
	auto pawn = GetPawn();
	if (!pawn)
	{
		return;
	}
	auto player = Cast<AIrone3DPlayer>(pawn);
	if (!player)
	{
		return;
	}
	const FVector toExitLocation = exitLocation - player->GetActorLocation();
	player->SetActorRotation(toExitLocation.ToOrientationRotator());
	// Disable gravity
	auto movementComp = player->GetCharacterMovement();
	check(movementComp);
	movementComp->GravityScale = 0.f;
	// Disable collision with other entities so we don't get stuck
	auto capsule = player->GetCapsuleComponent();
	check(capsule);
	///capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	capsule->SetCollisionResponseToChannel(ECC_ENEMY_PAWN, ECollisionResponse::ECR_Ignore);
	// Calculate the plane/ray which will end the movement script when we pass it
	autoMoveToLocationPoint = exitLocation;
	autoMoveToLocationRay = toExitLocation.GetSafeNormal();
	// Make moveForward(1.f) get called every frame until we pass the plane/ray
	// Make isFalling() always return false
	autoMoveToLocation = true;
}
void AIrone3DPlayerController::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	auto pawn = GetPawn();
	if (!pawn)
	{
		return;
	}
	auto player = Cast<AIrone3DPlayer>(pawn);
	if (!player)
	{
		return;
	}
	if (autoMoveToLocation)
	{
		const FVector toLocationPoint = autoMoveToLocationPoint - player->GetActorLocation();
		const float dotProd = FVector::DotProduct(toLocationPoint, autoMoveToLocationRay);
		if (dotProd <= 0)
		{
			autoMoveToLocation = false;
		}
		else
		{
			moveForward(1.f);
		}
	}
}
bool AIrone3DPlayerController::isAutoMoving() const
{
	return autoMoveToLocation;
}
void AIrone3DPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    check(InputComponent);
    InputComponent->BindAction("Jump", IE_Pressed,    this, &AIrone3DPlayerController::jumpPressed);
    InputComponent->BindAction("Jump", IE_Released,   this, &AIrone3DPlayerController::jumpReleased);
    InputComponent->BindAction("attack", IE_Pressed,  this, &AIrone3DPlayerController::attackPressed);
    InputComponent->BindAction("attack", IE_Released, this, &AIrone3DPlayerController::attackReleased);
    InputComponent->BindAxis("MoveForward", this, &AIrone3DPlayerController::moveForward);
    InputComponent->BindAxis("MoveRight",   this, &AIrone3DPlayerController::moveRight);
    InputComponent->BindAxis("Turn",        this, &AIrone3DPlayerController::turn);
    InputComponent->BindAxis("TurnRate",    this, &AIrone3DPlayerController::turnRate);
    InputComponent->BindAxis("LookUp",      this, &AIrone3DPlayerController::lookUp);
    InputComponent->BindAxis("LookUpRate",  this, &AIrone3DPlayerController::lookUpRate);
}
void AIrone3DPlayerController::jumpPressed()
{
	if (autoMoveToLocation) return;
    auto character = GetCharacter();
    if (character)
    {
        character->Jump();
    }
}
void AIrone3DPlayerController::jumpReleased()
{
    auto character = GetCharacter();
    if (character)
    {
        character->StopJumping();
    }
}
void AIrone3DPlayerController::attackPressed()
{
	if (autoMoveToLocation) return;
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->tryAttackStart();
    }
}
void AIrone3DPlayerController::attackReleased()
{
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->tryAttackEnd();
    }
}
void AIrone3DPlayerController::moveForward(float value)
{
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->moveForward(value);
    }
}
void AIrone3DPlayerController::moveRight(float value)
{
	if (autoMoveToLocation) return;
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->moveRight(value);
    }
}
void AIrone3DPlayerController::turn(float value)
{
	if (autoMoveToLocation) return;
    auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerYawInput(value);
    }
}
void AIrone3DPlayerController::turnRate(float value)
{
	if (autoMoveToLocation) return;
    if (FMath::Abs(value) < deadzone)
    {
        return;
    }
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->turnRate(value*baseTurnRate);
    }
}
void AIrone3DPlayerController::lookUp(float value)
{
	if (autoMoveToLocation) return;
    auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerPitchInput(value);
    }
}
void AIrone3DPlayerController::lookUpRate(float value)
{
	if (autoMoveToLocation) return;
    if (FMath::Abs(value) < deadzone)
    {
        return;
    }
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->lookUpRate(value*baseLookUpRate);
    }
}
