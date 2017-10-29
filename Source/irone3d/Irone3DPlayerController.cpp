// YOLO SWAG 420
#include "Irone3DPlayerController.h"
#include "Irone3DPlayer.h"
AIrone3DPlayerController::AIrone3DPlayerController()
    :baseTurnRate(45)
    ,baseLookUpRate(45)
    ,deadzone(0.01)
{
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
    auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->moveRight(value);
    }
}
void AIrone3DPlayerController::turn(float value)
{
    auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerYawInput(value);
    }
}
void AIrone3DPlayerController::turnRate(float value)
{
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
    auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerPitchInput(value);
    }
}
void AIrone3DPlayerController::lookUpRate(float value)
{
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