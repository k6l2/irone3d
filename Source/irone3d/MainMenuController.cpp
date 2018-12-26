// YOLO SWAG 420
#include "MainMenuController.h"
#include "MainMenuCameraPawn.h"
#include "Irone3DPlayer.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Kismet/GameplayStatics.h>
#include <Engine/World.h>
void AMainMenuController::Tick(float deltaSeconds)
{
	Super::Tick(deltaSeconds);
	AMainMenuCameraPawn*const camPawn = Cast<AMainMenuCameraPawn>(GetPawn());
	if (!ensure(camPawn)) return;
	AIrone3DPlayer*const playerActor = camPawn->getPlayerActor();
	if (!ensure(playerActor)) return;
	UWorld const*const world = GetWorld();
	if (!ensure(world)) return;
	if (playerWokenUp)
	{
		if (wakeUpDelaySeconds > 0)
		{
			wakeUpDelaySeconds -= deltaSeconds;
			camPawn->yawToPlayerActor();
		}
		else if(playerRunSeconds > 0)
		{
			playerRunSeconds -= deltaSeconds;
			camPawn->moveToPanTarget1();
			playerActor->moveForward(1.f);
			camPawn->yawToPlayerActor();
			if (playerActor->isFalling())
			{
				UCharacterMovementComponent*const playerCharMoveComp =
					Cast<UCharacterMovementComponent>(
						playerActor->GetMovementComponent());
				if (playerCharMoveComp)
				{
					playerCharMoveComp->Velocity *= 0.25f;
					playerRunSeconds = 0;
				}
			}
		}
		else if(fadeoutSeconds > 0)
		{
			fadeoutSeconds -= deltaSeconds;
			camPawn->moveToPanTarget2();
			camPawn->pitchStraightDown(deltaSeconds);
		}
		else
		{
			///TODO: fade out the screen
			UGameplayStatics::OpenLevel(world, "procGenPersist");
		}
	}
}
void AMainMenuController::BeginPlay()
{
	Super::BeginPlay();
	AMainMenuCameraPawn*const camPawn = Cast<AMainMenuCameraPawn>(GetPawn());
	if (!ensure(camPawn)) return;
	AIrone3DPlayer*const playerActor = camPawn->getPlayerActor();
	if (!ensure(playerActor)) return;
	camPawn->yawToPlayerActor();
	fadeoutSeconds = fadeoutSecondsTotal;
}
void AMainMenuController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (!ensure(InputComponent))
	{
		return;
	}
	InputComponent->BindAction("attack", EInputEvent::IE_Pressed, this, 
		&AMainMenuController::onAttackPressed);
}
void AMainMenuController::onAttackPressed()
{
	if (playerWokenUp)
	{
		return;
	}
	AMainMenuCameraPawn*const camPawn = Cast<AMainMenuCameraPawn>(GetPawn());
	if (!ensure(camPawn)) return;
	AIrone3DPlayer*const playerActor = camPawn->getPlayerActor();
	if (!ensure(playerActor)) return;
	playerActor->setSleeping(false);
	playerWokenUp = true;
}
