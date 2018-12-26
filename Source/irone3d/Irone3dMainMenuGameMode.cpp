// YOLO SWAG 420
#include "Irone3dMainMenuGameMode.h"
#include "MainMenuCameraPawn.h"
#include "MainMenuController.h"
AIrone3dMainMenuGameMode::AIrone3dMainMenuGameMode()
{
	DefaultPawnClass = AMainMenuCameraPawn::StaticClass();
	PlayerControllerClass = AMainMenuController::StaticClass();
}
