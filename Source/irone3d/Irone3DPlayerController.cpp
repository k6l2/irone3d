// YOLO SWAG 420
#include "Irone3DPlayerController.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Runtime/Engine/Classes/Components/CapsuleComponent.h>
#include <EngineGlobals.h>
#include <Runtime/Engine/Classes/Engine/Engine.h>
#include "Irone3DPlayer.h"
#include "irone3dGameMode.h"
#include "Irone3dGameState.h"
#include "irone3d.h"
#include <Engine/StaticMeshActor.h>
#include "RoomTransitionTrigger.h"
AIrone3DPlayerController::AIrone3DPlayerController()
    :baseTurnRate(45)
    ,baseLookUpRate(45)
    ,deadzone(0.01)
	,autoMoveToLocation(false)
{
}
void AIrone3DPlayerController::transitionExitToLocation(const FVector & exitLocation)
{
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(GetPawn());
	ensure(player);
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
	capsule->SetCollisionResponseToChannel(
		ECC_EnemyPawn, ECollisionResponse::ECR_Ignore);
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
	UWorld*const world = GetWorld();
	ensure(world);
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(GetPawn());
	ensure(player);
	AIrone3dGameState*const gs = world->GetGameState<AIrone3dGameState>();
	ensure(gs);
	if (autoMoveToLocation)
	{
		const FVector toLocationPoint = autoMoveToLocationPoint - player->GetActorLocation();
		const float dotProd = FVector::DotProduct(toLocationPoint, autoMoveToLocationRay);
		//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange,
		//	FString::Printf(TEXT("autoMoveToLocation! dotProd=%f"), dotProd));
		if (dotProd <= 0)
		{
			TArray<TWeakObjectPtr<AActor>> currRoomActorSet = 
				gs->getCurrentRoomActorSet();
			AStaticMeshActor* fallingDoor = nullptr;
			ARoomTransitionTrigger const* roomTransition = nullptr;
			//ASSUMPTION: in the boss room w/ a falling door, there is only one
			//	transition trigger in the room
			for (auto& actor : currRoomActorSet)
			{
				if (!actor.IsValid())
				{
					UE_LOG(LogTemp, Warning,
						TEXT("actor is invalid, skipping..."))
						continue;
				}
				AActor*const actorP = actor.Get();
				if (actorP->GetName() == "FallingDoor" &&
					actorP->IsA(AStaticMeshActor::StaticClass()))
				{
					fallingDoor = Cast<AStaticMeshActor>(actorP);
					continue;
				}
				if (actorP->IsA(ARoomTransitionTrigger::StaticClass()))
				{
					roomTransition = Cast<ARoomTransitionTrigger>(actorP);
				}
			}
			// if the room contains a AStaticMeshActor called "FallingDoor"
			//	AND the room contains a ARoomTransitionTrigger
			//	AND the falling door is not in the same position as the
			//		room transition trigger
			//	then, we need to move the falling door into place before ending
			//		the auto-movement cutscene
			if (fallingDoor && roomTransition)
			{
				FVector const toClosedPosition = 
					roomTransition->GetActorLocation() - 
						fallingDoor->GetActorLocation();
				if (FVector::DotProduct(toClosedPosition, 
						roomTransition->GetActorUpVector()) < 0)
				{
					closeDoorSpeed += 
						toClosedPosition.GetSafeNormal() * 1000 * deltaSeconds;
					fallingDoor->SetActorLocation(
						fallingDoor->GetActorLocation() + 
							closeDoorSpeed * deltaSeconds);
				}
				else
				{
					fallingDoor->SetActorLocation(
						roomTransition->GetActorLocation());
					onAutoMoveEnd();
				}
				player->cutSceneStopVelocity();
			}
			else
			{
				onAutoMoveEnd();
			}
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
bool AIrone3DPlayerController::isControlDisabled() const
{
	AIrone3DPlayer const*const ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
	return autoMoveToLocation ||
		(ironePlayer && ironePlayer->isKilled());
}
void AIrone3DPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();
    check(InputComponent);
    InputComponent->BindAction("Jump", IE_Pressed,    this, &AIrone3DPlayerController::jumpPressed);
    InputComponent->BindAction("Jump", IE_Released,   this, &AIrone3DPlayerController::jumpReleased);
    InputComponent->BindAction("attack", IE_Pressed,  this, &AIrone3DPlayerController::attackPressed);
    InputComponent->BindAction("attack", IE_Released, this, &AIrone3DPlayerController::attackReleased);
    InputComponent->BindAction("sprint", IE_Pressed,  this, &AIrone3DPlayerController::dashPressed);
    InputComponent->BindAction("sprint", IE_Released, this, &AIrone3DPlayerController::dashReleased);
    InputComponent->BindAxis("MoveForward", this, &AIrone3DPlayerController::moveForward);
    InputComponent->BindAxis("MoveRight",   this, &AIrone3DPlayerController::moveRight);
    InputComponent->BindAxis("Turn",        this, &AIrone3DPlayerController::turn);
    InputComponent->BindAxis("TurnRate",    this, &AIrone3DPlayerController::turnRate);
    InputComponent->BindAxis("LookUp",      this, &AIrone3DPlayerController::lookUp);
    InputComponent->BindAxis("LookUpRate",  this, &AIrone3DPlayerController::lookUpRate);
}
void AIrone3DPlayerController::jumpPressed()
{
	if (isControlDisabled()) return;
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
	if (isControlDisabled()) return;
	AIrone3DPlayer*const ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->tryAttackStart();
    }
}
void AIrone3DPlayerController::attackReleased()
{
	AIrone3DPlayer*const ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->tryAttackEnd();
    }
}
void AIrone3DPlayerController::dashPressed()
{
	if (isControlDisabled()) return;
	AIrone3DPlayer*const ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
	if (ironePlayer)
	{
		ironePlayer->dashStart();
	}
}
void AIrone3DPlayerController::dashReleased()
{
	AIrone3DPlayer*const ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
	if (ironePlayer)
	{
		ironePlayer->dashEnd();
	}
}
void AIrone3DPlayerController::moveForward(float value)
{
	if (isControlDisabled() && !autoMoveToLocation) return;
	auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->moveForward(value);
    }
}
void AIrone3DPlayerController::moveRight(float value)
{
	if (isControlDisabled()) return;
	auto ironePlayer = Cast<AIrone3DPlayer>(GetPawn());
    if (ironePlayer)
    {
        ironePlayer->moveRight(value);
    }
}
void AIrone3DPlayerController::turn(float value)
{
	if (isControlDisabled()) return;
	auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerYawInput(value);
    }
}
void AIrone3DPlayerController::turnRate(float value)
{
	if (isControlDisabled()) return;
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
	if (isControlDisabled()) return;
	auto pawn = GetPawn();
    if (pawn)
    {
        pawn->AddControllerPitchInput(value);
    }
}
void AIrone3DPlayerController::lookUpRate(float value)
{
	if (isControlDisabled()) return;
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
void AIrone3DPlayerController::onAutoMoveEnd()
{
	UWorld*const world = GetWorld();
	ensure(world);
	AGameModeBase*const gmb = world->GetAuthGameMode();
	ensure(gmb);
	Airone3dGameMode*const gm = Cast<Airone3dGameMode>(gmb);
	ensure(gm);
	APawn*const pawn = GetPawn();
	ensure(pawn);
	AIrone3DPlayer*const player = Cast<AIrone3DPlayer>(pawn);
	ensure(player);
	// re-enable gravity
	auto movementComp = player->GetCharacterMovement();
	check(movementComp);
	movementComp->GravityScale = 1.f;
	// Re-enable collision with other entities
	auto capsule = player->GetCapsuleComponent();
	check(capsule);
	///capsule->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	capsule->SetCollisionResponseToChannel(
		ECC_EnemyPawn, ECollisionResponse::ECR_Block);
	SetViewTarget(pawn);
	autoMoveToLocation = false;
	gm->onEndTransition();
}
