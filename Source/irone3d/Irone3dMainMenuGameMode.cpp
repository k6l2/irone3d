// YOLO SWAG 420
#include "Irone3dMainMenuGameMode.h"
#include "MainMenuCameraPawn.h"
#include "MainMenuController.h"
#include "ConstructorHelpers.h"
#include "WidgetMainMenuHud.h"
AIrone3dMainMenuGameMode::AIrone3dMainMenuGameMode()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> mainMenuHudBp(
		TEXT("/Game/GUI/WidgetBpMainMenuHud"));
	if (ensure(mainMenuHudBp.Succeeded()))
	{
		classHUD = mainMenuHudBp.Class;
	}
	DefaultPawnClass = AMainMenuCameraPawn::StaticClass();
	PlayerControllerClass = AMainMenuController::StaticClass();
}
void AIrone3dMainMenuGameMode::InitGame(const FString& MapName, 
	const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);
	UWorld*const world = GetWorld();
	if (ensure(classHUD))
	{
		widgetHUD = CreateWidget<UWidgetMainMenuHud>(world, classHUD);
		if (widgetHUD)
		{
			widgetHUD->AddToViewport();
		}
		widgetHUD->setFullscreenTextureColor(FLinearColor(0, 0, 0, 0));
	}
}
void AIrone3dMainMenuGameMode::setFullscreenTextureColor(
	const FLinearColor& color)
{
	widgetHUD->setFullscreenTextureColor(color);
}
